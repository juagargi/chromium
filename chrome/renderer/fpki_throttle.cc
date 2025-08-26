#include "chrome/renderer/fpki_throttle.h"

#include "chrome/renderer/fpki_bridge.h"
#include "services/network/public/cpp/resource_request.h"
#include "third_party/blink/public/platform/platform.h"
#include "url/gurl.h"
#include "url/url_constants.h"

RendererFpkiThrottle::RendererFpkiThrottle() = default;
RendererFpkiThrottle::~RendererFpkiThrottle() = default;

const char* RendererFpkiThrottle::NameForLoggingWillStartRequest() {
  return "RendererFpkiThrottle";
}

void RendererFpkiThrottle::WillStartRequest(network::ResourceRequest* request,
                                            bool* /*defer*/) {
  const GURL& url = request->url;
  if (!url.SchemeIs(url::kHttpsScheme)) {
    return;
  }

  fpki::RendererFpkiBridge::Get()->StartFetchIfNeeded(
      std::string(url.host_piece()));
}
