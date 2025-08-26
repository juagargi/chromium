// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/fpki_bridge.h"

#include "base/check.h"
#include "base/task/single_thread_task_runner.h"
#include "content/public/renderer/render_thread.h"

namespace fpki {

RendererFpkiBridge* RendererFpkiBridge::Get() {
  static base::NoDestructor<RendererFpkiBridge> instance;
  return instance.get();
}

RendererFpkiBridge::RendererFpkiBridge() = default;
RendererFpkiBridge::~RendererFpkiBridge() = default;

void RendererFpkiBridge::InitOnMainThread() {
  // Must be on renderer main.
  DCHECK(base::SingleThreadTaskRunner::HasCurrentDefault());
  main_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();

  if (!initialized_) {
    // Bind the browser interface via the render thread host binder.
    content::RenderThread::Get()->BindHostReceiver(
        fpki_.BindNewPipeAndPassReceiver());
    initialized_ = true;
  }
}

void RendererFpkiBridge::EnsureInitializedOnMain() {
  if (!initialized_) {
    InitOnMainThread();
  }
}

void RendererFpkiBridge::StartFetchIfNeeded(const std::string& host) {
  if (!main_runner_) {
    // First call came from a background sequence before init; schedule init+call.
    content::RenderThread::Get()->GetIOTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce([] { RendererFpkiBridge::Get()->InitOnMainThread(); }));
  }
  // Always hop to renderer main to use the mojo remote safely.
  if (main_runner_) {
    main_runner_->PostTask(
        FROM_HERE, base::BindOnce(&RendererFpkiBridge::StartFetchIfNeededOnMain,
                                  base::Unretained(this), host));
  } else {
    // Fallback: post to current default if present; otherwise drop.
    if (base::SingleThreadTaskRunner::HasCurrentDefault()) {
      base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
          FROM_HERE,
          base::BindOnce(&RendererFpkiBridge::StartFetchIfNeededOnMain,
                         base::Unretained(this), host));
    }
  }
}

void RendererFpkiBridge::StartFetchIfNeededOnMain(const std::string& host) {
  EnsureInitializedOnMain();
  if (fpki_.is_bound()) {
    fpki_->StartFetchIfNeeded(host);
  }
}

}  // namespace fpki
