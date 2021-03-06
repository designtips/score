// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ProtocolSettingsWidget.hpp"

namespace Device
{
ProtocolSettingsWidget::~ProtocolSettingsWidget() = default;
AddressDialog::~AddressDialog() = default;

Device::Node ProtocolSettingsWidget::getDevice() const
{
  return Device::Node{getSettings(), nullptr};
}

}
