// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/net/fpki_service.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/scoped_refptr.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/load_flags.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "url/gurl.h"

namespace {
  // TTL for the request cache.
constexpr base::TimeDelta kTtl = base::Minutes(5);
constexpr size_t kMaxBody = 256 * 1024;
constexpr char kFpkiEndpoint[] = MAPSERVER_ENDPOINT;
}

FpkiService* FpkiService::Get() {
  static base::NoDestructor<FpkiService> instance;
  return instance.get();
}

FpkiService::FpkiService() = default;
FpkiService::~FpkiService() = default;

void FpkiService::FetchIfNeeded(const std::string& host) {
  DLOG(INFO) << "deleteme +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+= FetchIfNeeded("<<
    host<<")";

  // Don’t ever fetch for the endpoint itself to avoid recursion.
  if (host.empty() || host == MAPSERVER_HOST) return;

  // Cheap cache/inflight gate on any thread:
  {
    base::AutoLock l(lock_);
    auto it = cache_.find(host);
    if (it != cache_.end() && (base::TimeTicks::Now() - it->second.ts) < kTtl)
      return; // Got it already.
    if (inflight_.count(host)) return; // Requesting a new one already.
    inflight_[host] = base::TimeTicks::Now();
  }

  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    // If coming from a different thread,
    content::GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&FpkiService::StartFetchOnUI,
                              base::Unretained(this),
                              host));
    return;
  }

  StartFetchOnUI(host);
}

void FpkiService::StartFetchOnUI(const std::string& host) {
  // Ensure we are called from the right thread, fail early if not:
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  auto* mgr = g_browser_process->system_network_context_manager();
  if (!mgr) {
    OnFetchDone(host, net::ERR_UNEXPECTED, nullptr);
    return;
  }

  scoped_refptr<network::SharedURLLoaderFactory> factory = mgr->GetSharedURLLoaderFactory();

  auto req = std::make_unique<network::ResourceRequest>();
  req->method = "GET";
  req->url = GURL(std::string(kFpkiEndpoint) + "?host=" + host);
  req->credentials_mode = network::mojom::CredentialsMode::kOmit;
  req->load_flags = net::LOAD_BYPASS_CACHE | net::LOAD_DISABLE_CACHE;

  static constexpr net::NetworkTrafficAnnotationTag kAnno =
      net::DefineNetworkTrafficAnnotation("fpki_side_fetch", R"(
        semantics { sender: "FPKI side fetch"
          description: "Gets FPKI validation data for the target host."
          trigger: "Whenever a page/resource request starts."
          data: "Hostname only." destination: OTHER }
        policy { cookies_allowed: NO })");

  auto loader = network::SimpleURLLoader::Create(std::move(req), kAnno);
  auto* raw = loader.get();
  // Commented out for now:
  // raw->SetRetryOptions(
  //     1, network::SimpleURLLoader::RETRY_ON_5XX |
  //            network::SimpleURLLoader::RETRY_ON_NETWORK_CHANGE);

  raw->DownloadToString(
    factory.get(),
    base::BindOnce([](
            std::string host,
            std::unique_ptr<network::SimpleURLLoader> keep,
            std::unique_ptr<std::string> body) {
              FpkiService::Get()->OnFetchDone(
                  host, keep->NetError(),
                  body ? std::make_unique<std::string>(*body) : nullptr);
            },
          host,
          std::move(loader)),
      kMaxBody);
}

void FpkiService::OnFetchDone(const std::string& host, int net_error,
                              std::unique_ptr<std::string> body) {
  base::AutoLock l(lock_);
  inflight_.erase(host);
  if (net_error == net::OK && body) {
    cache_[host] = CacheEntry{*body, base::TimeTicks::Now()};
  }
}

bool FpkiService::Lookup(const std::string& host, std::string* out) {
  base::AutoLock l(lock_);
  auto it = cache_.find(host);
  if (it == cache_.end()) return false;
  if ((base::TimeTicks::Now() - it->second.ts) > kTtl) return false;
  if (out) *out = it->second.body;
  return true;
}
