// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/net/fpki_throttle.h"
#include "chrome/browser/net/fpki_service.h"
#include "services/network/public/cpp/resource_request.h"
#include "url/gurl.h"
#include "url/url_constants.h"

namespace chrome::browser::net {


const char* FpkiThrottle::NameForLoggingWillStartRequest() {
  return "FpkiThrottle";
}

void FpkiThrottle::WillStartRequest(network::ResourceRequest* request,
                                    bool* /*defer*/) {

  const GURL& url = request->url;
  if (!url.SchemeIs(url::kHttpsScheme)) {
    return;
  }
  const std::string host = url.host();

  if (host.empty()) {
    return;
  }
  FpkiService::Get()->FetchIfNeeded(host);  // fire-and-forget, parallel to TLS
}

}
