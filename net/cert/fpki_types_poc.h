// net/cert/fpki_types_poc.h
#ifndef NET_CERT_FPKI_TYPES_POC_H_
#define NET_CERT_FPKI_TYPES_POC_H_

#include <string>
#include <vector>
#include <map>
#include <optional>

#include "base/component_export.h"
#include "net/cert/x509_certificate.h"
// No need to include "base/no_destructor.h" here, it's an implementation detail of the .cc file.
// No need to include "base/logging.h" or other implementation-specific headers here.

namespace net {

// Forward declaration for X509Certificate if only used as pointer/reference in this header.
// However, GetRootCaSubject takes X509Certificate*, so including its full header is generally safer
// and often already done by files that would include this one. If not, include "net/cert/x509_certificate.h".

namespace fpki_poc {

// --- FPKI Data Structures ---
// Using COMPONENT_EXPORT(NET) for classes/structs that might be used across component boundaries.
// If everything is within the same component (e.g., "net"), it might not be strictly necessary
// but is good practice if there's any chance of cross-component use. Remove if it causes issues
// and you're certain it's same-component only.
struct COMPONENT_EXPORT(NET) FpkiPolicyAttributes {
    std::vector<std::string> allowed_cas;
    std::vector<std::string> allowed_subdomains;
    std::vector<std::string> disallowed_subdomains;
    bool include_subdomains = false;
    bool domain_excluded_from_fpki = false;

    FpkiPolicyAttributes(); // Declare constructor
    ~FpkiPolicyAttributes(); // Declare destructor
    FpkiPolicyAttributes(const FpkiPolicyAttributes&); // Declare copy constructor
    FpkiPolicyAttributes& operator=(const FpkiPolicyAttributes&); // Declare copy assignment
    FpkiPolicyAttributes(FpkiPolicyAttributes&&) noexcept; // Declare move constructor
    FpkiPolicyAttributes& operator=(FpkiPolicyAttributes&&) noexcept; // Declare move assignment
};

struct COMPONENT_EXPORT(NET) FpkiPolicyCertificate {
    std::string policy_domain;
    FpkiPolicyAttributes attributes;

    FpkiPolicyCertificate();
    ~FpkiPolicyCertificate();
    FpkiPolicyCertificate(const FpkiPolicyCertificate&);
    FpkiPolicyCertificate& operator=(const FpkiPolicyCertificate&);
    FpkiPolicyCertificate(FpkiPolicyCertificate&&) noexcept;
    FpkiPolicyCertificate& operator=(FpkiPolicyCertificate&&) noexcept;
};

using FpkiPolicyChain = std::vector<FpkiPolicyCertificate>;

enum class COMPONENT_EXPORT(NET) FpkiPolicyOutcome { // Added COMPONENT_EXPORT
    SUCCESS,
    FAILURE_CA_NOT_ALLOWED,
    FAILURE_DOMAIN_RULE,
    DOMAIN_EXCLUDED_FROM_FPKI,
    NO_POLICY_FOUND
};

struct COMPONENT_EXPORT(NET) FpkiPolicyCheckResultDetails {
    FpkiPolicyOutcome outcome = FpkiPolicyOutcome::NO_POLICY_FOUND;
    std::vector<std::string> conflicting_ca_policy_domains;
    std::vector<std::string> conflicting_domain_rule_policy_domains;

    FpkiPolicyCheckResultDetails();
    ~FpkiPolicyCheckResultDetails();
    FpkiPolicyCheckResultDetails(const FpkiPolicyCheckResultDetails&);
    FpkiPolicyCheckResultDetails& operator=(const FpkiPolicyCheckResultDetails&);
    FpkiPolicyCheckResultDetails(FpkiPolicyCheckResultDetails&&) noexcept;
    FpkiPolicyCheckResultDetails& operator=(FpkiPolicyCheckResultDetails&&) noexcept;
};

// --- FPKI Cache (Simplified for PoC) ---
class COMPONENT_EXPORT(NET) FpkiPocPolicyCache {
public:
    FpkiPocPolicyCache();
    ~FpkiPocPolicyCache();

    FpkiPocPolicyCache(const FpkiPocPolicyCache&) = delete;
    FpkiPocPolicyCache& operator=(const FpkiPocPolicyCache&) = delete;

    std::optional<FpkiPolicyChain> GetApplicablePolicyChain(const std::string& hostname);

private:
    std::map<std::string, FpkiPolicyChain> policies_;
};

// --- Helper Function Declarations ---
COMPONENT_EXPORT(NET) std::string GetRootCaSubject(net::X509Certificate* cert_obj);

COMPONENT_EXPORT(NET) FpkiPolicyOutcome PerformFpkiPolicyCheck(
    net::X509Certificate* conn_chain_obj,
    const std::string& hostname,
    FpkiPocPolicyCache* policy_cache,
    FpkiPolicyCheckResultDetails* result_details);

// Singleton accessor declaration
COMPONENT_EXPORT(NET) FpkiPocPolicyCache* GetFpkiPocPolicyCacheInstance();

} // namespace fpki_poc
} // namespace net

#endif // NET_CERT_FPKI_TYPES_POC_H_
