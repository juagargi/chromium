#pragma once  // deleteme
#include "third_party/blink/public/common/loader/url_loader_throttle.h"
#include "chrome/common/fpki.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"

class RendererFpkiThrottle : public blink::URLLoaderThrottle {
 public:
  RendererFpkiThrottle();
  ~RendererFpkiThrottle() override;
  void WillStartRequest(network::ResourceRequest* request, bool* defer) override;
  const char* NameForLoggingWillStartRequest() override;

 private:
  void EnsureRemote();
  mojo::Remote<chrome::mojom::Fpki> fpki_;
};
