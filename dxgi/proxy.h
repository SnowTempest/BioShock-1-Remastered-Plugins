#pragma once

#ifndef DXGI_PROXY_H
#define DXGI_PROXY_H

namespace dxgi::proxy {
	void init(HMODULE this_dll);
	void free();
}

#endif