/*
    This file is part of solidity.

    solidity is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    solidity is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Unit tests for the iulia interpreter.
 */

#include <test/libjulia/Common.h>

#include <libjulia/interpreter/Interpreter.h>

#include <boost/test/unit_test.hpp>

#include <tuple>

using namespace std;
using namespace dev;
using namespace dev::julia;
using namespace dev::julia::test;
using namespace dev::solidity;


namespace
{
pair<bool, string> check(string const& _source, vector<string> const& _expectation)
{
	Block b = *parse(_source, false).first;
	InterpreterState state;
	Interpreter interpreter(state);
	try
	{
		interpreter(b);
	}
	catch (InterpreterTerminated const&)
	{
	}
	if (state.trace == _expectation)
		return std::make_pair(true, std::string{});
	std::string message =
			"Invalid trace\n"
			"   Result                                                           Expectation\n";
	for (size_t i = 0; i < std::max(_expectation.size(), state.trace.size()); ++i)
	{
		std::string result{i >= state.trace.size() ? string{"STOPPED"} : state.trace.at(i)};
		std::string expected{i >= _expectation.size() ? string{"STOPPED"} : _expectation.at(i)};
		message +=
			(result == expected ? "RES   " : "RES X ") +
			result +
			"\n" +
			"EXP   " +
			expected +
			"\n";
	}
	message += "\n\n{\n";
	for (size_t i = 0; i < state.trace.size(); ++i)
	{
		message += "\"" + state.trace[i] + "\"";
		if (i + 1 < state.trace.size())
			message += ",";
		message += "\n";
	}
	message += "}\n";
	return make_pair(false, message);
}
}

#define CHECK(_source, _expectation)\
do\
{\
	auto result = check(_source, _expectation);\
	BOOST_CHECK_MESSAGE(result.first, result.second);\
}\
while(false)

BOOST_AUTO_TEST_SUITE(IuliaInterpreter)

BOOST_AUTO_TEST_CASE(smoke_test)
{
	CHECK("{ }", {});
}

BOOST_AUTO_TEST_CASE(simple_mstore)
{
	CHECK("{ mstore(10, 11) }", {
		"MSTORE_AT_SIZE(10, 32) [000000000000000000000000000000000000000000000000000000000000000b]"
	});
}

BOOST_AUTO_TEST_SUITE_END()
