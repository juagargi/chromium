// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NET_FPKI_SERVICE_H_
#define CHROME_BROWSER_NET_FPKI_SERVICE_H_

#include <unordered_map>
#include "base/no_destructor.h"
#include "base/synchronization/lock.h"
#include "base/time/time.h"

#define MAPSERVER_HOST "mapserver.ethz.ch"
#define MAPSERVER_ENDPOINT "https://" MAPSERVER_HOST "/fpki"

class FpkiService {
 public:
  static FpkiService* Get();  // UI-thread singleton

  // Safe from any sequence; will hop to UI before touching NetworkContext.
  void FetchIfNeeded(const std::string& host);
  // No I/O; returns cached body if present/fresh.
  bool Lookup(const std::string& host, std::string* out);

 private:
  friend class base::NoDestructor<FpkiService>;

  FpkiService();
  ~FpkiService();

  void StartFetchOnUI(const std::string& host);
  void OnFetchDone(const std::string& host, int net_error,
                   std::unique_ptr<std::string> body);

  base::Lock lock_;
  struct CacheEntry { std::string body; base::TimeTicks ts; };
  std::unordered_map<std::string, CacheEntry> cache_ GUARDED_BY(lock_);
  std::unordered_map<std::string, base::TimeTicks> inflight_ GUARDED_BY(lock_);
};

#endif
