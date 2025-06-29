#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

struct Span {
    const char* filename;
    int         line;
    int         cols;
    int         cole;
    Span(const char* f, int l, int sc, int ec) : filename(strdup(f)), line(l), cols(sc), cole(ec) {}
    Span() {}
};

enum class DiagnosticType {
    Error,
    Warning,
    Info,
};

struct Diagnostic {
    DiagnosticType           kind;
    Span                     location;
    std::string              message;
    std::string              hint;
    Diagnostic*              ref;
    
    Diagnostic(DiagnosticType type, Span loc, std::string message, std::string help) : kind(type), location(loc),
    message(std::move(message)), hint(std::move(help)) {}
    
    Diagnostic(DiagnosticType type, Span loc, std::string message, std::string help, Diagnostic* ref) : kind(type), location(loc),
    message(std::move(message)), hint(std::move(help)), ref(ref) {}
};

struct ErrorManager {
    std::string              m_source;
    std::vector<std::string> lines;
    std::vector<Diagnostic>  errors;

    ErrorManager(std::string source): m_source(source) {
	std::stringstream s;
	s << source;
	std::string       line;
	while(std::getline(s, line)) lines.push_back(line);
    }
    
    void add(Diagnostic d) {
	this->errors.push_back(d);
    }

    void reportAll() {
	bool malformed = false;
	for (auto diag: errors) {
	    if (diag.kind == DiagnosticType::Error) malformed = true;
	    report(diag, false);
	}
	if (malformed) exit(1);
    }
    
    void report(Diagnostic d, bool noreturn) {
	int ln = d.location.line - 1;
	int cols = d.location.cols;
	int cole = d.location.cole;
	std::string line = lines.at(ln);
	std::string tag  = (d.kind == DiagnosticType::Error) ? "error" : (d.kind == DiagnosticType::Info)? "info": "warning";
	std::cout << d.location.filename << ":" << ln << ":" << cols << ": " << tag << ": " << d.message << "\n";
	std::cout << "   |\n";
	std::cout << ln << "  |  " << line << "\n";
	std::cout << "   |" << std::string(cols + 1, ' ');
	for(int i = cols; i < line.size() ; i++) {
	    if (i <= cole) {
		std::cout << "^";
	    }	   
	}
	std::cout << "\n";
	if (d.hint.size() > 0) {
	    std::cout << "   |" << std::string(cols + 1, ' ') << ":" << d.hint << "\n";
	}
	if (noreturn)
	exit(1);
    }
};
