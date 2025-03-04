#include <AggregateFunctions/AggregateFunctionFactory.h>
#include <AggregateFunctions/Helpers.h>
#include <AggregateFunctions/FactoryHelpers.h>
#include <AggregateFunctions/AggregateFunctionStatisticsSimple.h>


namespace DB
{
struct Settings;

namespace ErrorCodes
{
    extern const int ILLEGAL_TYPE_OF_ARGUMENT;
}

namespace
{

template <template <typename> typename FunctionTemplate>
AggregateFunctionPtr createAggregateFunctionStatisticsUnary(
    const std::string & name, const DataTypes & argument_types, const Array & parameters, const Settings *)
{
    assertNoParameters(name, parameters);
    assertUnary(name, argument_types);

    AggregateFunctionPtr res;
    const DataTypePtr & data_type = argument_types[0];
    if (isDecimal(data_type))
        res.reset(createWithDecimalType<FunctionTemplate>(*data_type, *data_type, argument_types));
    else
        res.reset(createWithNumericType<FunctionTemplate>(*data_type, argument_types));

    if (!res)
        throw Exception(ErrorCodes::ILLEGAL_TYPE_OF_ARGUMENT, "Illegal type {} of argument for aggregate function {}",
                        argument_types[0]->getName(), name);
    return res;
}

template <template <typename, typename> typename FunctionTemplate>
AggregateFunctionPtr createAggregateFunctionStatisticsBinary(
    const std::string & name, const DataTypes & argument_types, const Array & parameters, const Settings *)
{
    assertNoParameters(name, parameters);
    assertBinary(name, argument_types);

    AggregateFunctionPtr res(createWithTwoBasicNumericTypes<FunctionTemplate>(*argument_types[0], *argument_types[1], argument_types));
    if (!res)
        throw Exception(ErrorCodes::ILLEGAL_TYPE_OF_ARGUMENT, "Illegal types {} and {} of arguments for aggregate function {}",
            argument_types[0]->getName(), argument_types[1]->getName(), name);

    return res;
}

}

void registerAggregateFunctionsStatisticsSimple(AggregateFunctionFactory & factory)
{
    factory.registerFunction("varSamp", createAggregateFunctionStatisticsUnary<AggregateFunctionVarSampSimple>);
    factory.registerFunction("varPop", createAggregateFunctionStatisticsUnary<AggregateFunctionVarPopSimple>);
    factory.registerFunction("stddevSamp", createAggregateFunctionStatisticsUnary<AggregateFunctionStddevSampSimple>);
    factory.registerFunction("stddevPop", createAggregateFunctionStatisticsUnary<AggregateFunctionStddevPopSimple>);
    factory.registerFunction("skewSamp", createAggregateFunctionStatisticsUnary<AggregateFunctionSkewSampSimple>);
    factory.registerFunction("skewPop", createAggregateFunctionStatisticsUnary<AggregateFunctionSkewPopSimple>);
    factory.registerFunction("kurtSamp", createAggregateFunctionStatisticsUnary<AggregateFunctionKurtSampSimple>);
    factory.registerFunction("kurtPop", createAggregateFunctionStatisticsUnary<AggregateFunctionKurtPopSimple>);

    factory.registerFunction("covarSamp", createAggregateFunctionStatisticsBinary<AggregateFunctionCovarSampSimple>);
    factory.registerFunction("covarPop", createAggregateFunctionStatisticsBinary<AggregateFunctionCovarPopSimple>);
    factory.registerFunction("corr", createAggregateFunctionStatisticsBinary<AggregateFunctionCorrSimple>, AggregateFunctionFactory::CaseInsensitive);

    /// Synonims for compatibility.
    factory.registerAlias("VAR_SAMP", "varSamp", AggregateFunctionFactory::CaseInsensitive);
    factory.registerAlias("VAR_POP", "varPop", AggregateFunctionFactory::CaseInsensitive);
    factory.registerAlias("STDDEV_SAMP", "stddevSamp", AggregateFunctionFactory::CaseInsensitive);
    factory.registerAlias("STDDEV_POP", "stddevPop", AggregateFunctionFactory::CaseInsensitive);
    factory.registerAlias("COVAR_SAMP", "covarSamp", AggregateFunctionFactory::CaseInsensitive);
    factory.registerAlias("COVAR_POP", "covarPop", AggregateFunctionFactory::CaseInsensitive);
}

}
