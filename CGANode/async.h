#pragma once 

#include <nan.h>

using namespace v8;

class CBaseNotifyData
{
public:
	CBaseNotifyData(const uv_async_cb async_cb)
	{
		m_result = false;
		m_isolate = Isolate::GetCurrent();
		m_context.Reset(m_isolate, m_isolate->GetCurrentContext());
		m_async = new uv_async_t;
		m_timer = new uv_timer_t;
		uv_timer_init(uv_default_loop(), m_timer);
		uv_async_init(uv_default_loop(), m_async, async_cb);
		m_async->data = this;
		m_timer->data = this;
	}

	bool m_result;
	v8::Isolate* m_isolate;
	v8::Persistent<v8::Context> m_context;
	v8::Persistent<v8::Function> m_callback;
	uv_async_t *m_async;
	uv_timer_t *m_timer;
};

inline void FreeUVHandleCallBack(uv_handle_t *handle)
{
	delete handle;
}

//3000ms
#define NANO_SECONDS_TO_MS (1000000ull)

#define CGA_NOTIFY_MAX_CACHE_TIME (3000ull * NANO_SECONDS_TO_MS)
