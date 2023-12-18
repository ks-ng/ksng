#include "hdrs/httplib.h"
#include "utils.cpp"

struct Response {
	string url;
	string path;
	int status;
	bool redirects;
	string reason;
	string redirect;
	string body;

	string repr() {
		stringstream ss;
		ss << "<Response: " << status << " " << reason;
		if (redirects) {
			ss << " (redirects to " << redirect << ")";
		}
		ss << ">";
		return ss.str();
	}
};

class WebInterface {

	public:

		Response get(string url, string path) {
			httplib::Client cli(url);
			auto respptr = cli.Get(path);
			httplib::Response resp = *respptr;
			Response result;
			result.body = resp.body;
			result.reason = resp.reason;
			result.redirect = resp.location;
			result.redirects = (bool)(resp.location != (string)(""));
			result.status = resp.status;
			result.url = url;
			result.path = path;
			return result;
		}
};