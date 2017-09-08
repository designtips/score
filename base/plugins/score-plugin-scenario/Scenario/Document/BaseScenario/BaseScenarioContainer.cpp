// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Process/Algorithms/ProcessPolicy.hpp>
#include <Scenario/Process/Algorithms/VerticalMovePolicy.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>

#include <score/tools/std/Optional.hpp>

#include "BaseScenarioContainer.hpp"
#include <Process/TimeValue.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/model/Identifier.hpp>
namespace Scenario
{

BaseScenarioContainer::BaseScenarioContainer(no_init, QObject* parentObject)
    : m_parent{parentObject}
{
}

BaseScenarioContainer::BaseScenarioContainer(QObject* parentObject)
    : m_parent{parentObject}
{
  auto& stack = score::IDocument::documentContext(*m_parent).commandStack;
  m_startNode = new TimeSyncModel{Scenario::startId<TimeSyncModel>(),
                                  {0.2, 0.8},
                                  TimeVal::zero(),
                                  m_parent};
  m_endNode = new TimeSyncModel{Scenario::endId<TimeSyncModel>(),
                                {0.2, 0.8},
                                TimeVal::zero(),
                                m_parent};

  m_startEvent = new EventModel{Scenario::startId<EventModel>(),
                                m_startNode->id(),
                                {0.1, 0.15},
                                TimeVal::zero(),
                                m_parent};
  m_endEvent = new EventModel{Scenario::endId<EventModel>(),
                              m_endNode->id(),
                              {0.4, 0.6},
                              TimeVal::zero(),
                              m_parent};

  m_startState = new StateModel{Scenario::startId<StateModel>(),
                                m_startEvent->id(), 0, stack, m_parent};
  m_endState = new StateModel{Scenario::endId<StateModel>(), m_endEvent->id(),
                              0, stack, m_parent};

  m_interval = new IntervalModel{Id<IntervalModel>{0}, 0, m_parent};

  m_startNode->addEvent(m_startEvent->id());
  m_endNode->addEvent(m_endEvent->id());

  m_startEvent->addState(m_startState->id());
  m_endEvent->addState(m_endState->id());

  m_interval->setStartState(m_startState->id());
  m_interval->setEndState(m_endState->id());

  SetNextInterval(*m_startState, *m_interval);
  SetPreviousInterval(*m_endState, *m_interval);
}

BaseScenarioContainer::BaseScenarioContainer(
    const BaseScenarioContainer& source, QObject* parentObject)
    : m_parent{parentObject}
{
  auto& stack = score::IDocument::documentContext(*m_parent).commandStack;
  m_interval = new IntervalModel{*source.m_interval,
                                     source.m_interval->id(), m_parent};

  m_startNode = new TimeSyncModel{*source.m_startNode,
                                  source.m_startNode->id(), m_parent};
  m_endNode
      = new TimeSyncModel{*source.m_endNode, source.m_endNode->id(), m_parent};

  m_startEvent = new EventModel{*source.m_startEvent,
                                source.m_startEvent->id(), m_parent};
  m_endEvent
      = new EventModel{*source.m_endEvent, source.m_endEvent->id(), m_parent};

  m_startState = new StateModel{*source.m_startState,
                                source.m_startState->id(), stack, m_parent};
  m_endState = new StateModel{*source.m_endState, source.m_endState->id(),
                              stack, m_parent};

  SetPreviousInterval(*m_endState, *m_interval);
  SetNextInterval(*m_startState, *m_interval);
}

BaseScenarioContainer::~BaseScenarioContainer()
{
  delete m_interval;
  m_interval = nullptr;

  delete m_startState;
  m_startState = nullptr;
  delete m_endState;
  m_endState = nullptr;

  delete m_startEvent;
  m_startEvent = nullptr;
  delete m_endEvent;
  m_endEvent = nullptr;

  delete m_startNode;
  m_startNode = nullptr;
  delete m_endNode;
  m_endNode = nullptr;
}

IntervalModel*
BaseScenarioContainer::findInterval(const Id<IntervalModel>& id) const
{
  if (id == m_interval->id())
    return m_interval;
  return nullptr;
}

EventModel* BaseScenarioContainer::findEvent(const Id<EventModel>& id) const
{
  if (id == m_startEvent->id())
  {
    return m_startEvent;
  }
  else if (id == m_endEvent->id())
  {
    return m_endEvent;
  }
  else
  {
    return nullptr;
  }
}

TimeSyncModel*
BaseScenarioContainer::findTimeSync(const Id<TimeSyncModel>& id) const
{
  if (id == m_startNode->id())
  {
    return m_startNode;
  }
  else if (id == m_endNode->id())
  {
    return m_endNode;
  }
  else
  {
    return nullptr;
  }
}

StateModel* BaseScenarioContainer::findState(const Id<StateModel>& id) const
{
  if (id == m_startState->id())
  {
    return m_startState;
  }
  else if (id == m_endState->id())
  {
    return m_endState;
  }
  else
  {
    return nullptr;
  }
}

IntervalModel&
BaseScenarioContainer::interval(const Id<IntervalModel>& id) const
{
  SCORE_ASSERT(id == m_interval->id());
  return *m_interval;
}

EventModel& BaseScenarioContainer::event(const Id<EventModel>& id) const
{
  SCORE_ASSERT(id == m_startEvent->id() || id == m_endEvent->id());
  return id == m_startEvent->id() ? *m_startEvent : *m_endEvent;
}

TimeSyncModel&
BaseScenarioContainer::timeSync(const Id<TimeSyncModel>& id) const
{
  SCORE_ASSERT(id == m_startNode->id() || id == m_endNode->id());
  return id == m_startNode->id() ? *m_startNode : *m_endNode;
}

StateModel& BaseScenarioContainer::state(const Id<StateModel>& id) const
{
  SCORE_ASSERT(id == m_startState->id() || id == m_endState->id());
  return id == m_startState->id() ? *m_startState : *m_endState;
}

IntervalModel& BaseScenarioContainer::interval() const
{
  return *m_interval;
}

TimeSyncModel& BaseScenarioContainer::startTimeSync() const
{
  return *m_startNode;
}

TimeSyncModel& BaseScenarioContainer::endTimeSync() const
{
  return *m_endNode;
}

EventModel& BaseScenarioContainer::startEvent() const
{
  return *m_startEvent;
}

EventModel& BaseScenarioContainer::endEvent() const
{
  return *m_endEvent;
}

StateModel& BaseScenarioContainer::startState() const
{
  return *m_startState;
}

StateModel& BaseScenarioContainer::endState() const
{
  return *m_endState;
}
}