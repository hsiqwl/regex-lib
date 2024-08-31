#include <gtest/gtest.h>
#include "regex_engine.h"
#include "match_results.h"

TEST(regex_match, single_character){
    regex rgx("a");
    ASSERT_TRUE(rgx.match("a"));
    ASSERT_FALSE(rgx.match("b"));
}

TEST(regex_match, character_class){
    regex rgx("[a-c]");
    ASSERT_TRUE(rgx.match("a"));
    ASSERT_TRUE(rgx.match("b"));
    ASSERT_TRUE(rgx.match("c"));
    ASSERT_FALSE(rgx.match("d"));

    regex r("[ga-cde]");
    ASSERT_TRUE(r.match("a"));
    ASSERT_TRUE(r.match("b"));
    ASSERT_TRUE(r.match("c"));
    ASSERT_TRUE(r.match("d"));
    ASSERT_TRUE(r.match("e"));
    ASSERT_TRUE(r.match("g"));
    ASSERT_FALSE(r.match("f"));
    ASSERT_FALSE(r.match("ab"));
}

TEST(regex_match, concatenation){
    regex rgx("ab");
    ASSERT_TRUE(rgx.match("ab"));
    ASSERT_FALSE(rgx.match("a"));
    ASSERT_FALSE(rgx.match("b"));
    ASSERT_FALSE(rgx.match("ba"));
    ASSERT_FALSE(rgx.match("aba"));
}

TEST(regex_match, alternation){
    regex rgx("a|b");
    ASSERT_TRUE(rgx.match("a"));
    ASSERT_TRUE(rgx.match("b"));
    ASSERT_FALSE(rgx.match("ab"));
    ASSERT_FALSE(rgx.match("ba"));
}

TEST(regex_match, star){
    regex rgx("a*");
    ASSERT_TRUE(rgx.match(""));
    ASSERT_TRUE(rgx.match("a"));
    ASSERT_TRUE(rgx.match("aa"));
    ASSERT_TRUE(rgx.match("aaaaaaaaaaaaaaaaaaaaaaaaa"));
    ASSERT_FALSE(rgx.match("aaaaaaaaaaab"));
}

TEST(regex_match, plus){
    regex rgx("a+");
    ASSERT_TRUE(rgx.match("a"));
    ASSERT_TRUE(rgx.match("aa"));
    ASSERT_TRUE(rgx.match("aaaaaaaaaaaa"));
    ASSERT_FALSE(rgx.match(""));
}

TEST(regex_match, optional){
    regex rgx("a?");
    ASSERT_TRUE(rgx.match("a"));
    ASSERT_TRUE(rgx.match(""));
    ASSERT_FALSE(rgx.match("aa"));
}

TEST(regex_match, repeat){
    regex rgx("a{2,3}");
    ASSERT_TRUE(rgx.match("aa"));
    ASSERT_TRUE(rgx.match("aaa"));
    ASSERT_FALSE(rgx.match("a"));
    ASSERT_FALSE(rgx.match("aaaa"));

    regex r1("a{0,2}");
    ASSERT_TRUE(r1.match(""));
    ASSERT_TRUE(r1.match("aa"));
    ASSERT_TRUE(r1.match("a"));
    ASSERT_FALSE(r1.match("aaa"));
}

TEST(except, parenthesis){
    ASSERT_ANY_THROW(regex r("(a"));
    ASSERT_ANY_THROW(regex r("a{c,2}"));
    ASSERT_ANY_THROW(regex r("+5"));
    ASSERT_ANY_THROW(regex r("(a|)b"));
}

TEST(regex_match, emails){
    regex r("([a-z]|[A-Z]|[_0-9])+@([a-z]|[A-Z]|[0-9])+%.(ru|com)"); //for emails
    ASSERT_TRUE(r.match("example@gmail.com"));
    ASSERT_TRUE(r.match("example@yandex.ru"));
    ASSERT_FALSE(r.match("example@yandex.su"));
    ASSERT_FALSE(r.match("exampleGmail.com"));
}

TEST(regex_match, phone_numbers){
    regex r("(%+7|8)(-[0-9]{3,3}){2,2}(-[0-9]{2,2}){2,2}"); //for phone numbers
    ASSERT_TRUE(r.match("8-996-217-48-53"));
    ASSERT_TRUE(r.match("+7-893-392-13-64"));
    ASSERT_FALSE(r.match("8999-535-24-13"));
}

TEST(regex_match, dates) {
    regex r("(0[1-9]|[1-2][0-9]|3[0-1])%.(0[1-9]|1[0-2])%.(0{3,3}[1-9]|[1-9][0-9]{3,3})"); //for dates in format DD.MM.YYYY
    ASSERT_TRUE(r.match("27.04.2004"));
    ASSERT_TRUE(r.match("01.01.0001"));
    ASSERT_FALSE(r.match("00.00.0000"));
    ASSERT_FALSE(r.match("12.31.2024"));
}

TEST(regex_match, words_with_even_amount_of_letters){
    regex r("(([a-z]|[A-Z]){2,2})+");
    ASSERT_TRUE(r.match("aa"));
    ASSERT_TRUE(r.match("ldgk"));
    ASSERT_TRUE(r.match("skdgnojs"));
    ASSERT_FALSE(r.match("adskpdn"));
    ASSERT_FALSE(r.match("a"));
}

TEST(regex_to_string, single_character) {
    regex r1("a");
    regex r2(r1.get_initial_regex());
    ASSERT_EQ(r1.match("a"), r2.match("a"));
    ASSERT_EQ(r1.match("b"), r2.match("b"));
}

TEST(regex_to_string, character_class) {
    regex r1("[a-c]");
    regex r2(r1.get_initial_regex());
    ASSERT_EQ(r1.match("a"), r2.match("a"));
    ASSERT_EQ(r1.match("b"), r2.match("b"));
    ASSERT_EQ(r1.match("c"), r2.match("c"));
    ASSERT_EQ(r1.match("d"), r2.match("d"));
}

TEST(regex_to_string, concatenation){
    regex r1("abc");
    regex r2(r1.get_initial_regex());
    ASSERT_EQ(r1.match("abc"), r2.match("abc"));
    ASSERT_EQ(r1.match("ab"), r2.match("ab"));
    ASSERT_EQ(r1.match("ac"), r2.match("ac"));
}

TEST(regex_to_string, alternation){
    {
        regex r1("a|b");
        regex r2(r1.get_initial_regex());
        ASSERT_EQ(r1.match("a"), r2.match("a"));
        ASSERT_EQ(r1.match("b"), r2.match("b"));
        ASSERT_EQ(r1.match("c"), r2.match("c"));
    }
    {
        regex r1("a|b|c");
        regex r2(r1.get_initial_regex());
        ASSERT_EQ(r1.match("a"), r2.match("a"));
        ASSERT_EQ(r1.match("b"), r2.match("b"));
        ASSERT_EQ(r1.match("c"), r2.match("c"));
    }
}

TEST(regex_to_string, star) {
    regex r1("a*");
    regex r2(r1.get_initial_regex());
    ASSERT_EQ(r1.match(""), r2.match(""));
    ASSERT_EQ(r1.match("a"), r2.match("a"));
    ASSERT_EQ(r1.match("aaa"), r2.match("aaa"));
    ASSERT_EQ(r1.match("b"), r2.match("b"));
}

TEST(regex_to_string, plus){
    regex r1("a+");
    regex r2(r1.get_initial_regex());
    ASSERT_EQ(r1.match(""), r2.match(""));
    ASSERT_EQ(r1.match("a"), r2.match("a"));
    ASSERT_EQ(r1.match("aa"), r2.match("aa"));
    ASSERT_EQ(r1.match("aaaa"), r2.match("aaaa"));
    ASSERT_EQ(r1.match("b"), r2.match("b"));
}

TEST(regex_to_string, emails){
    regex r1("([a-z]|[A-Z]|[0-9])+@([a-z]|[A-Z]|[0-9])+%.(ru|com)"); //for emails
    regex r2(r1.get_initial_regex());
    ASSERT_EQ(r1.match("example@gmail.com"), r2.match("example@gmail.com"));
    ASSERT_EQ(r1.match("example@yandex.ru"), r2.match("example@yandex.ru"));
    ASSERT_EQ(r1.match("example@gmail.su"), r2.match("example@gmail.su"));
    ASSERT_EQ(r1.match("exampleGgmail.com"), r2.match("exampleGgmail.com"));
}

TEST(regex_to_string, phone_numbers){
    regex r1("(%+7|8)(-[0-9]{3,3}){2,2}(-[0-9]{2,2}){2,2}");
    regex r2(r1.get_initial_regex());//for phone numbers
    ASSERT_EQ(r1.match("8-996-217-48-53"), r2.match("8-996-217-48-53"));
    ASSERT_EQ(r1.match("+7-893-392-13-64"), r2.match("+7-893-392-13-64"));
    ASSERT_EQ(r1.match("8999-535-24-13"), r2.match("8999-535-24-13"));
}

TEST(regex_to_string, words_with_even_amount_of_letters){
    regex r1("(([a-z]|[A-Z]){2,2})+");
    regex r2(r1.get_initial_regex());
    ASSERT_EQ(r1.match("aa"), r2.match("aa"));
    ASSERT_EQ(r1.match("ldgk"), r2.match("ldgk"));
    ASSERT_EQ(r1.match("skdgnojs"), r2.match("skdgnojs"));
    ASSERT_EQ(r1.match("adskpdn"), r2.match("adskpdn"));
    ASSERT_EQ(r1.match("a"), r2.match("a"));
}

TEST(capturing, simple_group){
    regex r("a(bc)");
    match_result result(r, "abc");
    ASSERT_TRUE(result.size() == 2);
    ASSERT_TRUE(result[0] == "abc");
    ASSERT_TRUE(result[1] == "bc");
    ASSERT_TRUE(*result.begin() == "abc");
    ASSERT_TRUE(*(result.begin() + 1) == "bc");
    ASSERT_TRUE(result.begin() + 2 == result.end());
}

TEST(capturing, multiple_groups){
    regex r("a(bc)dgd(sdg)");
    match_result result(r, "abcdgdsdg");
    ASSERT_TRUE(result.size() == 3);
    ASSERT_TRUE(result[0] == "abcdgdsdg");
    ASSERT_TRUE(result[1] == "bc");
    ASSERT_TRUE(result[2] == "sdg");
    ASSERT_TRUE(*(result.begin()) == result[0]);
    ASSERT_TRUE(*(result.begin() + 1) == result[1]);
    ASSERT_TRUE(*(result.begin() + 2) == result[2]);
    ASSERT_TRUE(result.begin() + 3 == result.end());
}

TEST(capturing, failed_match){
    regex r("abdc");
    match_result result(r, "abcd");
    ASSERT_TRUE(result.size() == 0);
    ASSERT_TRUE(result.begin() == result.end());
}

TEST(capturing, star){
    regex r1("(a)*");
    match_result result(r1, "aaaaa");
    ASSERT_TRUE(result[0] == "aaaaa");
    ASSERT_TRUE(result[1] == "a");

    regex r2("(a|b)*");
    match_result res(r2, "aaabbab");
    ASSERT_TRUE(res[0] == "aaabbab");
    ASSERT_TRUE(res[1] == "b");
}

TEST(capturing, plus){
    {
        regex r("(a+)");
        match_result res1(r, "aaa");
        ASSERT_TRUE(res1.size() == 2);
        ASSERT_TRUE(res1[0] == "aaa");
        ASSERT_TRUE(res1[1] == "aaa");
    }
    {
        regex r("(a)+");
        match_result res1(r, "aaa");
        ASSERT_TRUE(res1.size() == 2);
        ASSERT_TRUE(res1[0] == "aaa");
        ASSERT_TRUE(res1[1] == "a");
    }
    {
        regex r("(a|b)+");
        match_result res1(r, "a");
        match_result res2(r, "b");
        match_result res3(r, "ab");
        match_result res4(r, "aba");
        ASSERT_TRUE(res1.size() == 2);
        ASSERT_TRUE(res2.size() == 2);
        ASSERT_TRUE(res3.size() == 2);
        ASSERT_TRUE(res4.size() == 2);
        ASSERT_TRUE(res1[1] == "a");
        ASSERT_TRUE(res2[1] == "b");
        ASSERT_TRUE(res3[1] == "b");
        ASSERT_TRUE(res4[1] == "a");
    }
}

TEST(capturing, repeat){
    {
        regex r("(a{2,3})");
        match_result res1(r, "aa");
        match_result res2(r, "aaa");
        ASSERT_TRUE(res1.size() == 2);
        ASSERT_TRUE(res2.size() == 2);
        ASSERT_TRUE(res1[1] == "aa");
        ASSERT_TRUE(res2[1] == "aaa");
    }
    {
        regex r("(a){2,3}");
        match_result res1(r, "aa");
        match_result res2(r, "aaa");
        ASSERT_TRUE(res1.size() == 2);
        ASSERT_TRUE(res2.size() == 2);
        ASSERT_TRUE(res1[1] == "a");
        ASSERT_TRUE(res2[1] == "a");
    }
}

TEST(capturing, optional){
    {
        regex r("a(b?)b");
        match_result res1(r, "ab");
        match_result res2(r, "abb");
        ASSERT_TRUE(res1.size() == 2);
        ASSERT_TRUE(res2.size() == 2);
        ASSERT_TRUE(res1[1].empty());
        ASSERT_TRUE(res2[1] == "b");
    }
    {
        regex r("a(b?)b?");
        match_result res1(r, "abb");
        match_result res2(r, "a");
        match_result res3(r, "ab");
        ASSERT_TRUE(res1[1] == "b");
        ASSERT_TRUE(res2[1].empty());
        ASSERT_TRUE(res3[1] == "b");
    }
}

TEST(capturing, emails){
    regex r("([a-z]|[A-Z]|[0-9])+@(([a-z]|[A-Z]|[0-9])+)%.(ru|com)"); //for emails
    match_result result(r, "absfls@gmail.com");
    ASSERT_TRUE(result[0] == "absfls@gmail.com");
    ASSERT_TRUE(result[1] == "s");
    ASSERT_TRUE(result[2] == "gmail");
    ASSERT_TRUE(result[3] == "l");
    ASSERT_TRUE(result[4] == "com");
}

TEST(capturing, dates) {
    regex r("(0[1-9]|[1-2][0-9]|3[0-1])%.(0[1-9]|1[0-2])%.(0{3,3}[1-9]|[1-9][0-9]{3,3})"); //for dates in format DD.MM.YYYY
    match_result result(r, "13.04.2035");
    ASSERT_TRUE(result.size() == 4);
    ASSERT_TRUE(result[0] == "13.04.2035");
    ASSERT_TRUE(result[1] == "13");
    ASSERT_TRUE(result[2] == "04");
    ASSERT_TRUE(result[3] == "2035");
}

TEST(capturing, phone_numbers){
    regex r1("(%+7|8)(-[0-9]{3,3}){2,2}(-[0-9]{2,2}){2,2}");
    match_result res1(r1, "8-996-217-48-53");
    match_result res2(r1, "+7-893-392-13-64");
    match_result res3(r1, "8999-535-24-13");
    ASSERT_TRUE(res1.size() == 4);
    ASSERT_TRUE(res2.size() == 4);
    ASSERT_TRUE(res3.size() == 0);
    ASSERT_TRUE(res1[1] == "8");
    ASSERT_TRUE(res2[1] == "+7");
    ASSERT_TRUE(res1[2] == "-217");
    ASSERT_TRUE(res2[2] == "-392");
    ASSERT_TRUE(res1[3] == "-53");
    ASSERT_TRUE(res2[3] == "-64");
}

TEST(capturing, ambigious){
    {
        regex r("a|(a)");
        match_result res(r, "a");
        ASSERT_TRUE(res.size() == 2);
        ASSERT_TRUE(res[0] == "a");
        ASSERT_TRUE(res[1] == "a");
    }
    {
        regex r("(a|aa)*");
        match_result res(r, "aa");
        ASSERT_TRUE(res.size() == 2);
        ASSERT_TRUE(res[1] == "aa");
        match_result res2(r, "aaa");
        ASSERT_TRUE(res2.size() == 2);
        ASSERT_TRUE(res2[1] == "aa");
    }
    {
        regex r("(a|ab*)b*");
        match_result res(r, "abb");
        ASSERT_TRUE(res.size() == 2);
        ASSERT_TRUE(res[1] == "abb");
    }
    {
        regex r("(a|ab*)b*");
        match_result res(r, "ab");
        ASSERT_TRUE(res.size() == 2);
        ASSERT_TRUE(res[1] == "ab");
    }
    {
        regex r("(a|ab+)b+");
        match_result res(r, "abb");
        ASSERT_TRUE(res.size() == 2);
        ASSERT_TRUE(res[1] == "ab");
    }
}

TEST(language_operations, complementation) {
    {
        regex r1("([a-z]|[A-Z]|[0-9])+@([a-z]|[A-Z]|[0-9])+%.(ru|com)");
        regex r2(r1.get_complemented_language());
        ASSERT_EQ(r1.match("example@gmail.com"), !r2.match("example@gmail.com"));
        ASSERT_EQ(r1.match("example@yandex.ru"), !r2.match("example@yandex.ru"));
        ASSERT_EQ(r1.match("example@gmail.su"), !r2.match("example@gmail.su"));
        ASSERT_EQ(r1.match("exampleGgmail.com"), !r2.match("exampleGgmail.com"));
    }
    {
        regex r1("(%+7|8)(-[0-9]{3,3}){2,2}(-[0-9]{2,2}){2,2}");
        regex r2(r1.get_complemented_language());
        ASSERT_EQ(r1.match("8-996-217-48-53"), !r2.match("8-996-217-48-53"));
        ASSERT_EQ(r1.match("+7-893-392-13-64"), !r2.match("+7-893-392-13-64"));
        ASSERT_EQ(r1.match("8999-535-24-13"), !r2.match("8999-535-24-13"));
    }
    {
        regex r1("(([a-z]|[A-Z]){2,2})+");
        regex r2(r1.get_complemented_language());
        ASSERT_EQ(r1.match("aa"), !r2.match("aa"));
        ASSERT_EQ(r1.match("ldgk"), !r2.match("ldgk"));
        ASSERT_EQ(r1.match("skdgnojs"), !r2.match("skdgnojs"));
        ASSERT_EQ(r1.match("adskpdn"), !r2.match("adskpdn"));
        ASSERT_EQ(r1.match("a"), !r2.match("a"));
    }
}

TEST(language_operations, reverse_language) {
    {
        regex r1("([a-z]|[A-Z]|[0-9])+@([a-z]|[A-Z]|[0-9])+%.(ru|com)");
        regex r2(r1.get_reverse_language());
        std::string s1 = "example@gmail.com";
        std::string s2 = "example@yandex.ru";
        std::string s3 = "example@gmail.su";
        std::string s4 = "exampleGgmail.com";
        ASSERT_EQ(r1.match(s1), r2.match(std::string{s1.rbegin(), s1.rend()}));
        ASSERT_EQ(r1.match(s2), r2.match(std::string{s2.rbegin(), s2.rend()}));
        ASSERT_EQ(r1.match(s3), r2.match(std::string{s3.rbegin(), s3.rend()}));
        ASSERT_EQ(r1.match(s4), r2.match(std::string{s4.rbegin(), s4.rend()}));
    }
    {
        regex r1("(%+7|8)(-[0-9]{3,3}){2,2}(-[0-9]{2,2}){2,2}");
        regex r2(r1.get_reverse_language());
        std::string s1 = "8-996-217-48-53";
        std::string s2 = "+7-893-392-13-64";
        std::string s3 = "8999-535-24-13";
        ASSERT_EQ(r1.match(s1), r2.match(std::string{s1.rbegin(), s1.rend()}));
        ASSERT_EQ(r1.match(s2), r2.match(std::string{s2.rbegin(), s2.rend()}));
        ASSERT_EQ(r1.match(s3), r2.match(std::string{s3.rbegin(), s3.rend()}));
    }
    {
        regex r1("(([a-z]|[A-Z]){2,2})+");
        regex r2(r1.get_reverse_language());
        std::string s1 = "aa";
        std::string s2 = "ldgk";
        std::string s3 = "skdgnojs";
        std::string s4 = "adskpdn";
        std::string s5 = "a";
        ASSERT_EQ(r1.match(s1), r2.match(std::string{s1.rbegin(), s1.rend()}));
        ASSERT_EQ(r1.match(s2), r2.match(std::string{s2.rbegin(), s2.rend()}));
        ASSERT_EQ(r1.match(s3), r2.match(std::string{s3.rbegin(), s3.rend()}));
        ASSERT_EQ(r1.match(s4), r2.match(std::string{s4.rbegin(), s4.rend()}));
        ASSERT_EQ(r1.match(s5), r2.match(std::string{s5.rbegin(), s5.rend()}));
    }
}