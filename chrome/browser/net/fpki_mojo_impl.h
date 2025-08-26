// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_NET_FPKI_MOJO_IMPL_H_
#define CHROME_BROWSER_NET_FPKI_MOJO_IMPL_H_

#include "chrome/common/fpki.mojom.h"
#include "mojo/public/cpp/bindings/receiver.h"

class FpkiMojoImpl : public chrome::mojom::Fpki {
 public:
  FpkiMojoImpl();
  ~FpkiMojoImpl() override;

  void Bind(mojo::PendingReceiver<chrome::mojom::Fpki> receiver);

  // From interface chrome::mojom::Fpki:
  void StartFetchIfNeeded(const std::string& host) override;

 private:
  void OnDisconnect();
  mojo::Receiver<chrome::mojom::Fpki> receiver_{this};
};

#endif
