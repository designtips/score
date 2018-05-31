#include "PutInLoop.hpp"

#include <Loop/LoopProcessModel.hpp>
#include <Scenario/Commands/Interval/InsertContentInInterval.hpp>

namespace Loop
{

void EncapsulateInLoop(
    const Scenario::ProcessModel& scenar,
    const score::CommandStackFacade& stack)
{
  using namespace Scenario;
  using namespace Scenario::Command;
  Scenario::Command::Macro disp{new Encapsulate, stack.context()};

  CategorisedScenario cat{scenar};
  if (cat.selectedIntervals.empty())
    return;
  if (cat.selectedIntervals.size() == 1)
  {
    // Just move all the processes in the pattern
    const IntervalModel& source_itv = *cat.selectedIntervals.front();
    auto itv_json = score::marshall<JSONObject>(source_itv);

    disp.clearInterval(source_itv);
    auto& loop =
        static_cast<Loop::ProcessModel&>(
          *disp.createProcess(source_itv, Metadata<ConcreteKey_k, Loop::ProcessModel>::get(), {}));

    disp.insertInInterval(
        std::move(itv_json), loop.intervals()[0], ExpandMode::Scale);
    disp.commit();
  }
  else
  {
    auto objects = copySelectedScenarioElements(scenar, cat);

    auto e = EncapsulateElements(disp, cat, scenar);
    if (!e.interval)
      return;

    auto& loop_parent_itv = *e.interval;

    auto& loop = disp.createProcess<Loop::ProcessModel>(loop_parent_itv, {});

    auto& itv = loop.intervals()[0];

    {
      // Add a sub-scenario
      auto& sub_scenar = disp.createProcess<Scenario::ProcessModel>(itv, {});

      disp.pasteElements(
          sub_scenar, objects, Scenario::Point{{}, 0.1});

      // Merge inside
      for (TimeSyncModel& sync : sub_scenar.timeSyncs)
      {
        if (&sync != &sub_scenar.startTimeSync()
            && sync.date() == TimeVal::zero())
        {
          disp.mergeTimeSyncs(
              sub_scenar, sync.id(), sub_scenar.startTimeSync().id());
          break;
        }
      }
    }

    // Resize the slot to fit the existing elements
    disp.resizeSlot(
        loop_parent_itv,
        SlotPath{loop_parent_itv, 0, Slot::RackView::SmallView},
        175 + (e.bottomY - e.topY) * 400);

    disp.commit();
  }
}
}
