#include "stats.h"
//#include <iostream>

void Stats::AddMethod(string_view method) {
	if (method == "POST" || method == "GET" || method == "PUT" || method == "DELETE")
		method_data[method] += 1;
	else
		method_data["UNKNOWN"] += 1;
}

void Stats::AddUri(string_view uri) {
	if (uri == "/" || uri == "/order" || uri == "/product" || uri == "/basket" ||
	    uri == "/help")
		uri_data[uri] += 1;
	else
		uri_data["unknown"] += 1;
}

const map<string_view, int> &Stats::GetMethodStats() const {
	return method_data;
}

const map<string_view, int> &Stats::GetUriStats() const {
	return uri_data;
}

HttpRequest ParseRequest(string_view line) {
	HttpRequest ret;
	line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));
	auto sep1 = line.find_first_of(' ');
	auto sep2 = line.find_last_of(' ');
	ret.method = line.substr(0, sep1);
	ret.uri = line.substr(sep1 + 1, sep2 - sep1 - 1);
	ret.protocol = line.substr(sep2 + 1, string::npos);
	//std::cout << ret.method << "|" << ret.uri << "|" << ret.protocol << std::endl;
	return ret;
}