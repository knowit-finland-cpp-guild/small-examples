#include <iostream>

class ArgumentsAndMembers
{
public:
    ArgumentsAndMembers(
            const std::string a,
            const std::string d,
            const std::string aa,
            const std::string dd) :
        a("member"), b(a), c("member"), d(c)
    {
        this->aa = "member";
        this->bb = aa;
        this->cc = "member";
        this->dd = cc;

//        aa = "member"; // Will not compile: aa const argument
//        bb = aa;       // This is ok:       bb not const argument
//        cc = "member"; // This is ok:       cc not const argument
//        dd = cc;       // Will not compile: dd const argument
    }

    friend std::ostream& operator<<(
            std::ostream& os,
            const ArgumentsAndMembers& t)
    {
        os << "a : "   << t.a  << ", b : " << t.b
           << ", c : " << t.c  << ", d : " << t.d << std::endl;
        os << "aa: "   << t.aa << ", bb: " << t.bb
           << ", cc: " << t.cc << ", dd: " << t.dd;
        return os;
    }
	
private:
    std::string a = "init";
    std::string b = "init";
    std::string c = "init";
    std::string d = "init";
    std::string aa = "init";
    std::string bb = "init";
    std::string cc = "init";
    std::string dd = "init";
};

int main()
{
    ArgumentsAndMembers t(
            "argument",
            "argument",
            "argument",
            "argument");
    
    // Printout:
    // a : member, b : argument, c : member, d : member
    // aa: member, bb: argument, cc: member, dd: member
    //
    // Explanation:
    // In the initializer list:
    // a exists as an argument and as a member
    // argument a is set to "argument"
    // member a is set to "member"
    // member b gets the value of a, from the argument
    // c exists as a member and is set to "member"
    // member d gets the value of member c
    // 
    // In the constructor scope:
    // Variables aa, bb, cc, and dd get the same behavior
    // as a, b, c, d, respectively,
    // in the constructor scope as in the initializer list
    // when the this pointer is used in the assignment.
    //
    // Summary:
    // In the initializer list, with overlapping naming
    // of argument and member, the precedence is
    // 1: argument, 2: member.
    // Using the this pointer, the same can be achieved
    // within the constructor scope.
    std::cout << t << std::endl;
}

