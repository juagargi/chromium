// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NET_FPKI_THROTTLE_H_
#define CHROME_BROWSER_NET_FPKI_THROTTLE_H_

// #include <memory>
#include "third_party/blink/public/common/loader/url_loader_throttle.h"

namespace chrome::browser::net {

class FpkiThrottle : public blink::URLLoaderThrottle {
 public:
  // static std::unique_ptr<blink::URLLoaderThrottle> Create();

  // blink::URLLoaderThrottle:
  void WillStartRequest(network::ResourceRequest* request,
                        bool* defer) override;
  const char* NameForLoggingWillStartRequest() override;
  void DetachFromCurrentSequence() override {}
};

}
#endif
