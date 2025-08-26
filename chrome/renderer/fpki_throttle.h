// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_RENDERER_FPKI_THROTTLE_H_
#define CHROME_RENDERER_FPKI_THROTTLE_H_

#include "third_party/blink/public/common/loader/url_loader_throttle.h"

class RendererFpkiThrottle : public blink::URLLoaderThrottle {
 public:
  RendererFpkiThrottle();
  ~RendererFpkiThrottle() override;
  void WillStartRequest(network::ResourceRequest* request,
                        bool* defer) override;
  const char* NameForLoggingWillStartRequest() override;
};

#endif
