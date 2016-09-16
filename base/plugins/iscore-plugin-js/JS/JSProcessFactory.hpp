#pragma once
#include <Process/GenericProcessFactory.hpp>

#include <Process/ProcessFactory.hpp>
#include <Process/LayerModel.hpp>
#include <JS/JSProcessModel.hpp>
#include <JS/JSStateProcess.hpp>
#include <JS/JSProcessMetadata.hpp>

#include <Process/StateProcessFactory.hpp>

namespace JS
{
using Layer = Process::LayerModel_T<JS::ProcessModel>;
}

LAYER_METADATA(
        ,
        JS::Layer,
        "1dc410bb-c2bd-41f4-a011-5382d61c9b07",
        "JSLayer",
        "JSLayer"
        )
namespace JS
{
using ProcessFactory = Process::GenericProcessModelFactory<JS::ProcessModel>;
using LayerFactory = Process::GenericDefaultLayerFactory<JS::Layer>;

using StateProcessFactory = Process::StateProcessFactory_T<JS::StateProcess>;
}
