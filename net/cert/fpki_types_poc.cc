// net/cert/fpki_types_poc.cc
#include "net/cert/fpki_types_poc.h"

#include "base/logging.h"
#include "base/no_destructor.h"
#include "base/strings/string_util.h" // For base::EndsWith
#include "net/base/url_util.h"        // For net::GetDomainAndRegistry
#include "net/cert/x509_util.h"       // For bssl::UpRef


namespace net::fpki_poc {

// --- FpkiPolicyAttributes Definitions ---
FpkiPolicyAttributes::FpkiPolicyAttributes() = default;
FpkiPolicyAttributes::~FpkiPolicyAttributes() = default;
FpkiPolicyAttributes::FpkiPolicyAttributes(const FpkiPolicyAttributes&) = default;
FpkiPolicyAttributes& FpkiPolicyAttributes::operator=(const FpkiPolicyAttributes&) = default;
FpkiPolicyAttributes::FpkiPolicyAttributes(FpkiPolicyAttributes&&) noexcept = default;
FpkiPolicyAttributes& FpkiPolicyAttributes::operator=(FpkiPolicyAttributes&&) noexcept = default;

// --- FpkiPolicyCertificate Definitions ---
FpkiPolicyCertificate::FpkiPolicyCertificate() = default;
FpkiPolicyCertificate::~FpkiPolicyCertificate() = default;
FpkiPolicyCertificate::FpkiPolicyCertificate(const FpkiPolicyCertificate&) = default;
FpkiPolicyCertificate& FpkiPolicyCertificate::operator=(const FpkiPolicyCertificate&) = default;
FpkiPolicyCertificate::FpkiPolicyCertificate(FpkiPolicyCertificate&&) noexcept = default;
FpkiPolicyCertificate& FpkiPolicyCertificate::operator=(FpkiPolicyCertificate&&) noexcept = default;

// --- FpkiPolicyCheckResultDetails Definitions ---
FpkiPolicyCheckResultDetails::FpkiPolicyCheckResultDetails() = default;
FpkiPolicyCheckResultDetails::~FpkiPolicyCheckResultDetails() = default;
FpkiPolicyCheckResultDetails::FpkiPolicyCheckResultDetails(const FpkiPolicyCheckResultDetails&) = default;
FpkiPolicyCheckResultDetails& FpkiPolicyCheckResultDetails::operator=(const FpkiPolicyCheckResultDetails&) = default;
FpkiPolicyCheckResultDetails::FpkiPolicyCheckResultDetails(FpkiPolicyCheckResultDetails&&) noexcept = default;
FpkiPolicyCheckResultDetails& FpkiPolicyCheckResultDetails::operator=(FpkiPolicyCheckResultDetails&&) noexcept = default;


// --- FpkiPocPolicyCache Definitions ---
FpkiPocPolicyCache::FpkiPocPolicyCache() {

    //From JSON config file
    // Pydantic-like parsing of JSON config file can be done here if needed.
    // Hardcoded policies
    FpkiPolicyCertificate google_com_policy;
    google_com_policy.policy_domain = "google.com";
    google_com_policy.attributes.allowed_cas.push_back("C=US, O=Google Trust Services LLC, CN=GTS Root R1");
    google_com_policy.attributes.allowed_cas.push_back("C=US, O=Google Trust Services LLC, CN=GTS Root R2");
    google_com_policy.attributes.allowed_cas.push_back("C=BE, O=GlobalSign nv-sa, OU=Root CA, CN=GlobalSign Root CA");
    google_com_policy.attributes.include_subdomains = true;
    policies_["google.com"] = {google_com_policy};

    FpkiPolicyCertificate wikipedia_org_policy;
    wikipedia_org_policy.policy_domain = "wikipedia.org";
    wikipedia_org_policy.attributes.allowed_cas.push_back("O=Let's Encrypt, CN=ISRG Root X1");
    wikipedia_org_policy.attributes.allowed_cas.push_back("C=US, O=Internet Security Research Group, CN=ISRG Root X1");
    wikipedia_org_policy.attributes.allowed_cas.push_back("C=US, O=DigiCert Inc, CN=DigiCert Global Root G2");
    wikipedia_org_policy.attributes.include_subdomains = true;
    policies_["wikipedia.org"] = {wikipedia_org_policy};

    FpkiPolicyCertificate example_com_policy;
    example_com_policy.policy_domain = "example.com";
    example_com_policy.attributes.allowed_cas.push_back("O=Let's Encrypt, CN=ISRG Root X1");
    example_com_policy.attributes.include_subdomains = true;
    policies_["example.com"] = {example_com_policy};

    FpkiPolicyCertificate secure_example_com_policy;
    secure_example_com_policy.policy_domain = "secure.example.com";
    secure_example_com_policy.attributes.allowed_cas.push_back("C=US, O=DigiCert Inc, CN=DigiCert Global Root G2");
    policies_["secure.example.com"] = {secure_example_com_policy};

    FpkiPolicyCertificate test_disallow_policy;
    test_disallow_policy.policy_domain = "test-disallow.com";
    test_disallow_policy.attributes.allowed_cas.push_back("C=US, O=DigiCert Inc, CN=DigiCert Global Root G2");
    test_disallow_policy.attributes.include_subdomains = true;
    policies_["test-disallow.com"] = {test_disallow_policy};

    FpkiPolicyCertificate excluded_domain_policy;
    excluded_domain_policy.policy_domain = "fpki-excluded.com";
    excluded_domain_policy.attributes.domain_excluded_from_fpki = true;
    excluded_domain_policy.attributes.include_subdomains = true;
    policies_["fpki-excluded.com"] = {excluded_domain_policy};

    FpkiPolicyCertificate main_site_policy;
    main_site_policy.policy_domain = "main-site-disallows-subs.com";
    main_site_policy.attributes.allowed_cas.push_back("O=Let's Encrypt, CN=ISRG Root X1");
    main_site_policy.attributes.include_subdomains = true;
    main_site_policy.attributes.disallowed_subdomains.push_back("admin.main-site-disallows-subs.com");
    main_site_policy.attributes.disallowed_subdomains.push_back("api.main-site-disallows-subs.com");
    policies_["main-site-disallows-subs.com"] = {main_site_policy};

    FpkiPolicyCertificate wildcard_policy;
    wildcard_policy.policy_domain = "*";
    wildcard_policy.attributes.allowed_cas.push_back("O=Let's Encrypt, CN=ISRG Root X1");
    wildcard_policy.attributes.allowed_cas.push_back("Test Root CA");
    wildcard_policy.attributes.include_subdomains = true;
    wildcard_policy.attributes.disallowed_subdomains.push_back("test.wildcard-disallowed.com");
    policies_["*"] = {wildcard_policy};
}

FpkiPocPolicyCache::~FpkiPocPolicyCache() = default;

std::optional<FpkiPolicyChain> FpkiPocPolicyCache::GetApplicablePolicyChain(const std::string& hostname) {
    std::string current_host = hostname;

    auto it_exact = policies_.find(current_host);
    if (it_exact != policies_.end()) {
        return it_exact->second;
    }

    std::string temp_host = current_host;
    size_t dot_pos;
    while ((dot_pos = temp_host.find('.')) != std::string::npos) {
        std::string parent_domain = temp_host.substr(dot_pos + 1);
        if (parent_domain.empty() || parent_domain == temp_host) break;

        auto it_parent = policies_.find(parent_domain);
        if (it_parent != policies_.end()) {
            const auto& parent_policy_chain = it_parent->second;
            if (!parent_policy_chain.empty()) {
                const auto& top_parent_policy_attrs = parent_policy_chain[0].attributes;

                if (top_parent_policy_attrs.include_subdomains) {
                    bool is_disallowed = false;
                    for (const auto& disallowed_sub_fqdn : top_parent_policy_attrs.disallowed_subdomains) {
                        if (hostname == disallowed_sub_fqdn) {
                            is_disallowed = true;
                            break;
                        }
                    }
                    if (is_disallowed) {
                        return FpkiPolicyChain();
                    }
                    return parent_policy_chain;
                }
            }
        }
        temp_host = parent_domain;
        if (temp_host.find('.') == std::string::npos && !temp_host.empty() && parent_domain != temp_host ) {
             auto it_tld = policies_.find(temp_host);
             if (it_tld != policies_.end()) {
                  const auto& tld_policy_chain = it_tld->second;
                  if (!tld_policy_chain.empty()) {
                      const auto& top_tld_policy_attrs = tld_policy_chain[0].attributes;
                       if (top_tld_policy_attrs.include_subdomains) {
                            bool is_disallowed_by_tld = false;
                            for (const auto& disallowed_sub_fqdn : top_tld_policy_attrs.disallowed_subdomains) {
                                if (hostname == disallowed_sub_fqdn) {
                                    is_disallowed_by_tld = true;
                                    break;
                                }
                            }
                            if (is_disallowed_by_tld) return FpkiPolicyChain();
                           return tld_policy_chain;
                       }
                  }
             } else {
                break;
             }
        } else if (temp_host.find('.') == std::string::npos) {
            break;
        }
    }
    auto it_wildcard = policies_.find("*");
    if (it_wildcard != policies_.end()) {
        const auto& wildcard_policy_chain = it_wildcard->second;
        if(!wildcard_policy_chain.empty()){
            const auto& wildcard_attrs = wildcard_policy_chain[0].attributes;
            bool is_disallowed_by_wildcard = false;
             for (const auto& disallowed_sub_fqdn : wildcard_attrs.disallowed_subdomains) {
                if (hostname == disallowed_sub_fqdn) {
                    is_disallowed_by_wildcard = true;
                    break;
                }
            }
            if(is_disallowed_by_wildcard) return FpkiPolicyChain();
            return wildcard_policy_chain;
        }
    }
    return std::nullopt;
}

// --- Helper Function Definitions ---
std::string GetRootCaSubject(net::X509Certificate* cert_obj) {
    if (!cert_obj) {
        return "FPKI_PoC_Error_Null_Cert_Object";
    }
    const auto& intermediates_raw = cert_obj->intermediate_buffers();
    if (intermediates_raw.empty()) {
        return cert_obj->issuer().GetDisplayName();
    }
    scoped_refptr<X509Certificate> highest_sent_cert =
        X509Certificate::CreateFromBuffer(
            bssl::UpRef(intermediates_raw.back().get()), {});
    if (highest_sent_cert) {
        return highest_sent_cert->subject().GetDisplayName();
    }
    return "FPKI_PoC_Error_Parsing_Intermediate";
}

FpkiPolicyOutcome PerformFpkiPolicyCheck(
    net::X509Certificate* conn_chain_obj,
    const std::string& hostname,
    FpkiPocPolicyCache* policy_cache,
    FpkiPolicyCheckResultDetails* result_details) {

    std::optional<FpkiPolicyChain> policy_chain_opt = policy_cache->GetApplicablePolicyChain(hostname);

    if (!policy_chain_opt) {
        result_details->outcome = FpkiPolicyOutcome::NO_POLICY_FOUND;
        return result_details->outcome;
    }
    if (policy_chain_opt->empty()){
        result_details->outcome = FpkiPolicyOutcome::FAILURE_DOMAIN_RULE;
        result_details->conflicting_domain_rule_policy_domains.push_back(hostname);
        DLOG(INFO) << "FPKI PoC: Domain rule VIOLATION for " << hostname << " due to explicit disallow in policy hierarchy.";
        return result_details->outcome;
    }

    const FpkiPolicyChain& policy_chain = *policy_chain_opt;
    std::string root_ca_subject_str = GetRootCaSubject(conn_chain_obj);
    const FpkiPolicyCertificate& applicable_policy = policy_chain[0];

    if (applicable_policy.attributes.domain_excluded_from_fpki) {
        result_details->outcome = FpkiPolicyOutcome::DOMAIN_EXCLUDED_FROM_FPKI;
        return result_details->outcome;
    }

    bool ca_allowed_by_fpki_policy = applicable_policy.attributes.allowed_cas.empty();
    if (!ca_allowed_by_fpki_policy) {
        for (const auto& allowed_ca : applicable_policy.attributes.allowed_cas) {
            if (root_ca_subject_str == allowed_ca) {
                ca_allowed_by_fpki_policy = true;
                break;
            }
        }
    }

    if (!ca_allowed_by_fpki_policy) {
        result_details->outcome = FpkiPolicyOutcome::FAILURE_CA_NOT_ALLOWED;
        result_details->conflicting_ca_policy_domains.push_back(applicable_policy.policy_domain);
        DLOG(INFO) << "FPKI PoC: CA '" << root_ca_subject_str << "' not allowed for domain '" << hostname
                   << "' by FPKI policy for pattern '" << applicable_policy.policy_domain << "'. Allowed FPKI CAs:";
        if (applicable_policy.attributes.allowed_cas.empty()){
             DLOG(INFO) << "    (Policy had empty allowed_cas list, but CA check failed - indicates logic error)";
        } else {
            for(const auto& ca : applicable_policy.attributes.allowed_cas) { DLOG(INFO) << "    - " << ca; }
        }
        return result_details->outcome;
    }

    result_details->outcome = FpkiPolicyOutcome::SUCCESS;
    return result_details->outcome;
}

// Singleton accessor definition
FpkiPocPolicyCache* GetFpkiPocPolicyCacheInstance() {
    static base::NoDestructor<FpkiPocPolicyCache> instance;
    return instance.get();
}

} // namespace net::fpki_poc
