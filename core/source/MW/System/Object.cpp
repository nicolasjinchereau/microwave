/*--------------------------------------------------------------*
*  Copyright (c) 2022 Nicolas Jinchereau. All rights reserved.  *
*--------------------------------------------------------------*/

module Microwave.System.Object;
import Microwave.System.App;
import Microwave.System.Executor;
import Microwave.System.SyncExecutor;

namespace mw {
inline namespace system {
namespace detail {

Task<sptr<Object>> GetAssetAsync(
    const UUID& uuid,
    const sptr<Executor>& executor)
{
    sptr<Object> ret = co_await App::Get()->GetAssetLibrary()->GetAssetAsync(uuid, executor);
    co_return ret;
}

} // detail

Task<void> ObjectLinker::LinkAsync(
    ObjectLinker* linker,
    const sptr<Executor>& executor)
{
    if (linker)
    {
        std::vector<Task<void>> tasks;
        tasks.reserve(linker->links.size());

        for (auto& link : linker->links)
            tasks.push_back(link->LinkAsync(linker, executor));

        for (auto& task : tasks)
            co_await task;
    }
}

void ObjectLinker::Link(ObjectLinker* linker)
{
    if (linker)
    {
        for (auto& link : linker->links) {
            link->LinkAsync(linker, SyncExecutor::GetInstance()).GetResult();
        }
    }
}

} // system
} // mw
