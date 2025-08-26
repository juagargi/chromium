// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_RENDERER_FPKI_BRIDGE_H_
#define CHROME_RENDERER_FPKI_BRIDGE_H_

#include <string>

#include "base/no_destructor.h"
#include "chrome/common/fpki.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"

namespace fpki {

// Lives in the renderer process. Mojo pipe is bound on the renderer MAIN thread.
// StartFetchIfNeeded() is safe to call from ANY sequence: it posts to main.
class RendererFpkiBridge {
 public:
  static RendererFpkiBridge* Get();

  // Must be called on the renderer main thread during init.
  void InitOnMainThread();

  // Safe from any sequence. If not initialized yet, this will post the init.
  void StartFetchIfNeeded(const std::string& host);

 private:
  friend class base::NoDestructor<RendererFpkiBridge>;
  RendererFpkiBridge();
  ~RendererFpkiBridge();

  void EnsureInitializedOnMain();
  void StartFetchIfNeededOnMain(const std::string& host);

  // Bound and used ONLY on renderer main thread.
  mojo::Remote<chrome::mojom::Fpki> fpki_;

  // Renderer main thread task runner. Set during InitOnMainThread().
  scoped_refptr<base::SingleThreadTaskRunner> main_runner_;

  bool initialized_ = false;
};

}  // namespace fpki



#endif  // CHROME_RENDERER_FPKI_BRIDGE_H_
