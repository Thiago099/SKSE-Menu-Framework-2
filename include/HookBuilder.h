#pragma once
#include <type_traits>





class HookBuilder {
private:

    enum WriteKind { kCall, kBranch };
    class GenericHookItem {
    public:
        virtual void Install(SKSE::Trampoline& tranpoline) = 0;
        virtual size_t GetTrampolineSize() = 0;
        virtual ~GenericHookItem() {}
    };
    template <class T, std::size_t N, std::size_t TS, WriteKind Kind>
    class HookItem : public GenericHookItem {
        int _ae_id;
        int _ae_offset;
        int _se_id;
        int _se_offset;
        int _vr_id;
        int _vr_offset;

    public:
        HookItem(int se_id, int se_offset, int ae_id, int ae_offset, int vr_id = 0, int vr_offset = 0) {
            _se_id = se_id;
            _ae_id = ae_id;
            _se_offset = se_offset;
            _ae_offset = ae_offset;
            _vr_id = vr_id;
            _vr_offset = vr_offset;
        }
        size_t GetTrampolineSize() { return TS; }
        void Install(SKSE::Trampoline& tranpoline) {
            if (REL::Module::IsAE()) {
                if (_ae_offset == 0x0) {
                    logger::error("Missing AE offset");
                    return;
                }
                if (_ae_id == 0) {
                    logger::error("Missing AE id");
                    return;
                }
            } else {
                if (_se_offset == 0x0) {
                    logger::error("Missing SE offset");
                    return;
                }
                if (_se_id == 0) {
                    logger::error("Missing SE id");
                    return;
                }
            }
            REL::Relocation<std::uintptr_t> function{REL::RelocationID(_se_id, _ae_id, _vr_id)};
            if constexpr (Kind == WriteKind::kCall) {
                T::originalFunction =
                    tranpoline.write_call<N>(function.address() + REL::Relocate(_se_offset, _ae_offset, _vr_offset), T::thunk);
            } else {
                T::originalFunction = tranpoline.write_branch<N>(
                    function.address() + REL::Relocate(_se_offset, _ae_offset, _vr_offset), T::thunk);
            }
        };
    };

    std::vector<GenericHookItem*> items;

public:
        ~HookBuilder() {
        }
        template <class T, std::size_t N, std::size_t TS>
		 void AddCall(int se_id, int se_offset, int ae_id, int ae_offset, int vr_id = 0, int vr_offset = 0) {
            if constexpr (N != 5 && N != 6) {
                static_assert(false && N, "invalid call size");
            }

            const auto newItem = new HookItem<T, N, TS, WriteKind::kCall>(se_id, se_offset, ae_id, ae_offset, vr_id, vr_offset);
            items.push_back(newItem);
		}
        template <class T, std::size_t N, std::size_t TS>
        void AddBranch(int se_id, int se_offset, int ae_id, int ae_offset, int vr_id = 0, int vr_offset = 0) {
            if constexpr (N != 5 && N != 6) {
                static_assert(false && N, "invalid call size");
            }

            const auto newItem = HookItem<T, N, TS, WriteKind::kBranch>(se_id, se_offset, ae_id, ae_offset, vr_id, vr_offset);
            items.push_back(newItem);
        }
        void AddBuilder(HookBuilder* builder) {
            items.insert(items.end(), builder->items.begin(), builder->items.end());
            delete builder;
        }

		void Install() {
            auto& trampoline = SKSE::GetTrampoline();
            trampoline.create(std::accumulate(items.begin(), items.end(), size_t(0),
                                [](size_t sum, GenericHookItem* cls) { return sum + cls->GetTrampolineSize(); }));
            for (auto& item : items) {
                item->Install(trampoline);
                delete item;
			}
            items.clear();
		}
};