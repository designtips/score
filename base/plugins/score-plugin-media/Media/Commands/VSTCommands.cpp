#include "VSTCommands.hpp"
#include <Media/Effect/VST/VSTEffectModel.hpp>
namespace Media::VST
{
SetVSTControl::SetVSTControl(const VSTControlInlet& obj, float newval)
: m_path{obj}
, m_old{obj.value()}
, m_new{newval}
{
}

SetVSTControl::~SetVSTControl() { }

void SetVSTControl::undo(const score::DocumentContext& ctx) const
{
  m_path.find(ctx).setValue(m_old);
}

void SetVSTControl::redo(const score::DocumentContext& ctx) const
{
  m_path.find(ctx).setValue(m_new);
}

void SetVSTControl::update(const VSTControlInlet& obj, float newval)
{
  m_new = newval;
}

void SetVSTControl::serializeImpl(DataStreamInput& stream) const
{
  stream << m_path << m_old << m_new;
}

void SetVSTControl::deserializeImpl(DataStreamOutput& stream)
{
  stream >> m_path >> m_old >> m_new;
}


CreateVSTControl::CreateVSTControl(const VSTEffectModel& obj, int fxNum, float value)
: m_path{obj}
, m_fxNum{fxNum}
, m_val{value}
{
}

CreateVSTControl::~CreateVSTControl() { }

void CreateVSTControl::undo(const score::DocumentContext& ctx) const
{
  VSTEffectModel& obj = m_path.find(ctx);
  auto it = obj.controls.find(m_fxNum);
  SCORE_ASSERT(it != obj.controls.end());
  auto ctrl = it->second;
  obj.controls.erase(it);
  for(auto it = obj.m_inlets.begin(); it != obj.m_inlets.end(); ++it)
  {
    if(*it == ctrl)
    {
      obj.m_inlets.erase(it);
      break;
    }
  }
  emit obj.controlRemoved(ctrl->id());
  delete ctrl;
}

void CreateVSTControl::redo(const score::DocumentContext& ctx) const
{
  m_path.find(ctx).on_addControl(m_fxNum, m_val);
}

void CreateVSTControl::serializeImpl(DataStreamInput& stream) const
{
  stream << m_path << m_fxNum << m_val;
}

void CreateVSTControl::deserializeImpl(DataStreamOutput& stream)
{
  stream >> m_path >> m_fxNum >> m_val;
}

}