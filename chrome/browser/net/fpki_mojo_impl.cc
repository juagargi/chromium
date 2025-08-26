// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/net/fpki_mojo_impl.h"
#include "chrome/browser/net/fpki_service.h"

FpkiMojoImpl::FpkiMojoImpl() = default;
FpkiMojoImpl::~FpkiMojoImpl() = default;

void FpkiMojoImpl::Bind(mojo::PendingReceiver<chrome::mojom::Fpki> receiver) {
  receiver_.Bind(std::move(receiver));
  receiver_.set_disconnect_handler(
      base::BindOnce(&FpkiMojoImpl::OnDisconnect, base::Unretained(this)));
}

void FpkiMojoImpl::OnDisconnect() { delete this; }

void FpkiMojoImpl::StartFetchIfNeeded(const std::string& host) {
  // We are in the BROWSER process; FpkiService will hop to UI if needed.
  FpkiService::Get()->FetchIfNeeded(host);
}
