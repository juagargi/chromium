

# Build

Instructions here:
- https://github.com/chromium/chromium/blob/main/docs/linux/build_instructions.md#get-the-code
- https://github.com/chromium/chromium/blob/main/docs/linux/build_instructions.md#docker
- https://wiki.gentoo.org/wiki/Project:Chromium/How_to_make_a_Chromium_tarball

If not done yet, obtain the source code and dependencies using `depot_tools` to
be able to build. Otherwise, if using only `git` to check out the repository,
the third party code won't be available, and most likely Chromium won't build.

## Simplified developer build
The script `run-docker.sh` creates a docker image with all necessary
dependencies in it, and build scripts ready inside it.
It then runs a long-lived container to build Chromium inside,
by allowing the user to exec a bash inside and run the build script:
```bash
./run-docker.sh
... OUTPUT OF DOCKER COMPOSE BUILDING THE IMAGE ...

chromium-build.sh
... OUTPUT OF BUILD PROCESS ...
```

## Debugging
The project can be interactively debugged with Qt Creator or VS Code.
Follow instruction here:
- [Qt Creator](https://github.com/juagargi/chromium/blob/main/docs/qtcreator.md)
- [VS Code](https://github.com/juagargi/chromium/blob/main/docs/vscode.md)


# Planned Work Roadmap
(temporary)

1. Create a set of configuration variables for F-PKI, that should be loaded
    upon Chromium boot. These will include:
    - Mapserver locations.
    - User F-PKI preferences. (CAs, etc.)
2. Find where TLS certificates are validated and there, add a request to the
    mapserver to retrieve the landscape for the domain in question.
    The landscape only contains SHA256 values of the existing certificates for
    the domain in question.
3. Validate the proof of presence (inclusion or absence) given by the mapserver.
4. With the landscape, Chromium can determine which certificates it is missing
    from its F-PKI cache (or Chromium cache), it will determine now which
    certificates it needs to retrieve from the mapserver and push them into
    the cache.
5. With all certificates in the Chromium cache, it can validate them now.
    (CA signature, expiration time.) Chromium can now write an entry to its configuration
    with the SHA256 value of those certificates that it knows will be permanently invalid.
6. With all the remaining valid certificates, Chromium applies now the user preferences
    and determines if the certificate from TLS is still valid.


## Findings
1. The SSL connection is verified in `ssl_client_socket_impl.cc`, method `SSLClientSocketImpl::VerifyCertCallback`.
  That function is called from BoringSSL to perform the custom validation.
2. There is a certificate verifier `CertVerifier` in `net/cert/cert_verifier.h`, but it is prevented from performing any network fetches. It runs asynchronously.
3. There is a `CertVerifyProc` that runs synchronously, and is called
  from e.g. `MultiThreadedCertVerifier` (an implementation of `CertVerifier`).
4. There are network fetches for OCSP but they are disabled by default.
  See `cert_net_fetcher_url_request.h` for details.
5. The best way to make a request to the mapserver is via a `URLLoaderThrottle`, that would have to be added
  using the methods `ChromeContentBrowserClient::CreateURLLoaderThrottles` and
  `URLLoaderThrottleProviderImpl::CreateThrottles`. Both methods run once per browsing request, meaning that there is no
  hard registration to run code when the page is being loaded unless we run both methods.


## Modifications to the Original Source Code
