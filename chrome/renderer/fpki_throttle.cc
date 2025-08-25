#include "chrome/renderer/fpki_throttle.h"

// #include "content/public/common/service_names.mojom.h"  //
// kBrowserServiceName
#include "content/public/renderer/render_thread.h"
#include "services/network/public/cpp/resource_request.h"
// #include "services/service_manager/public/cpp/connector.h"
#include "third_party/blink/public/platform/platform.h"
#include "url/gurl.h"
#include "url/url_constants.h"

RendererFpkiThrottle::RendererFpkiThrottle() = default;
RendererFpkiThrottle::~RendererFpkiThrottle() = default;

const char* RendererFpkiThrottle::NameForLoggingWillStartRequest() {
  return "RendererFpkiThrottle";
}

void RendererFpkiThrottle::EnsureRemote() {
  if (fpki_.is_bound()) {
    return;
  }
  content::RenderThread::Get()->BindHostReceiver(
      fpki_.BindNewPipeAndPassReceiver());

  // // blink::Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
  // //     fpki_.BindNewPipeAndPassReceiver());
  // // Old Service Manager path: ask the browser process via Connector.
  // service_manager::Connector* connector =
  //     content::RenderThread::Get()->GetConnector();
  // if (connector) {
  //   connector->BindInterface(content::mojom::kBrowserServiceName,
  //                            fpki_.BindNewPipeAndPassReceiver());
  // }
  // // content::RenderThread::Get()->GetBrowserInterfaceBroker()
  // //     ->GetInterface(fpki_.BindNewPipeAndPassReceiver());
}

void RendererFpkiThrottle::WillStartRequest(network::ResourceRequest* request,
                                            bool* /*defer*/) {
  const GURL& url = request->url;
  if (!url.SchemeIs(url::kHttpsScheme)) {
    return;
  }
  EnsureRemote();
  if (fpki_.is_bound()) {
    fpki_->StartFetchIfNeeded(std::string(url.host()));
  }
}
