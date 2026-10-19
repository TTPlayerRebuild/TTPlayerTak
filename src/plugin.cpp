#include "ttpcomm/audio_format_probe_impl.h"
#include "common.h"
namespace ttp::tak {
HMODULE module{};
StandardContent standard_content() {
    for(const wchar_t *name:{L"ttpctrl.dll",L"soundcore.dll",static_cast<const wchar_t*>(nullptr)})
        if(auto h=GetModuleHandleW(name))if(auto p=GetProcAddress(h,"CreateStdContent"))return reinterpret_cast<StandardContent>(p);
    return nullptr;
}
class Factory final : public ReaderCreator, public ttp::format::ProbeSupport {
protected:
    DWORD ProbeFamilies() const noexcept override { return ttp::format::Tak; }
private:

    LONG refs_{1};
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id,void **out) override {
        if (!out) return E_POINTER;
        if (IsEqualIID(id, ttp::format::iid_probe)) { *out=static_cast<ttp::format::Probe*>(this); AddRef(); return S_OK; }

        if(!out)return E_POINTER; *out=nullptr;
        if(!same(id,IID_IUnknown)&&!same(id,cat_reader))return E_NOINTERFACE;
        *out=static_cast<ReaderCreator*>(this);AddRef();return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override{return InterlockedIncrement(&refs_);}
    ULONG STDMETHODCALLTYPE Release() override{auto n=InterlockedDecrement(&refs_);if(!n)delete this;return n;}
    HRESULT STDMETHODCALLTYPE Create(void **out) override{return make_reader(out);}
    HRESULT STDMETHODCALLTYPE Name(wchar_t **out) override{return text(L"TAK Reader",out);}
    HRESULT STDMETHODCALLTYPE Extensions(wchar_t **out) override{return text(L"TAK 音频文件(*.tak)",out);}
};
class SoundAddIn final : public AddIn {
    LONG refs_{1};
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID id,void **out) override {
        if(!out)return E_POINTER;*out=nullptr;
        if(!same(id,IID_IUnknown)&&!same(id,iid_addin))return E_NOINTERFACE;
        *out=static_cast<AddIn*>(this);AddRef();return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override{return InterlockedIncrement(&refs_);}
    ULONG STDMETHODCALLTYPE Release() override{auto n=InterlockedDecrement(&refs_);if(!n)delete this;return n;}
    HRESULT STDMETHODCALLTYPE Enum(DWORD index,GUID *category,void **out) override{
        if(!out)return E_POINTER;*out=nullptr;if(!category)return E_POINTER;
        if(index)return E_INVALIDARG;
        return protect([&]()->HRESULT{*out=static_cast<ReaderCreator*>(new Factory);*category=cat_reader;return S_OK;});
    }
};
}
extern "C" HRESULT WINAPI ttpGetSoundAddIn(void **out){
    if(!out)return E_POINTER;*out=nullptr;
    return ttp::tak::protect([&]()->HRESULT{*out=static_cast<ttp::tak::AddIn*>(new ttp::tak::SoundAddIn);return S_OK;});
}
BOOL WINAPI DllMain(HINSTANCE h,DWORD reason,LPVOID){
    if(reason==DLL_PROCESS_ATTACH){ttp::tak::module=h;DisableThreadLibraryCalls(h);}return TRUE;
}

