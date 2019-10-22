#include <doctest/doctest.h>

#include <as/Src.h>
#include <as/Scan.h>

#include <mn/Defer.h>
#include <mn/IO.h>
#include <mn/Path.h>

#include <mn/Defer.h>
#include <mn/IO.h>
#include <mn/Path.h>

mn::Str
file_content_normalized(const mn::Str& filename)
{
	auto content = mn::file_content_str(filename, mn::memory::tmp());
	auto normalized = mn::str_tmp();
	mn::buf_reserve(normalized, content.count);
	for (size_t i = 0; i < content.count; ++i)
		if (content[i] != '\r')
			mn::buf_push(normalized, content[i]);
	mn::str_null_terminate(normalized);
	return normalized;
}

TEST_CASE("scan tests")
{
	auto files = mn::path_entries(mn::path_join(mn::str_tmp(), TEST_DIR, "scan"), mn::memory::tmp());

	std::sort(begin(files), end(files), [](const auto& a, const auto& b) { return a.name < b.name; });

	for(size_t i = 2; i < files.count; i += 2)
	{
		if (files[i].kind == mn::Path_Entry::KIND_FOLDER)
			continue;

		auto input = mn::path_join(mn::str_tmp(), TEST_DIR, "scan", files[i].name);
		auto output = mn::path_join(mn::str_tmp(), TEST_DIR, "scan", files[i + 1].name);
		auto expected = file_content_normalized(output);
		auto answer = mn::str_tmp();

		auto unit = as::src_from_file(input.ptr);
		mn_defer(as::src_free(unit));

		if (as::scan(unit) == false)
			answer = as::src_errs_dump(unit, mn::memory::tmp());
		else
			answer = as::src_tkns_dump(unit, mn::memory::tmp());

		if(expected != answer)
		{
			mn::printerr("TEST CASE: input '{}', output '{}'\n", input, output);
			mn::printerr("EXPECTED\n{}\nFOUND\n{}", expected, answer);
		}
		CHECK(expected == answer);
	}
}