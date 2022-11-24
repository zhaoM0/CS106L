### 模板函数

------

```c++
// 包含默认类型的模板函数
template <typename Type=int>
Type myMin(Type a, Type b) {
    return a < b ? a : b;
}
// 调用时显式调用
cout << myMin<int>(2, 3) << endl;
```

一种更加智能的模板函数，注意里面返回值使用 `auto` 来自行推断类型

```c++
template <typename T, typename U>
auto smarterMin(T a, U b) {
    return a < b ? a : b;
}
```

如同模板类一样，模板函数在使用之前是不会被编译的，只有在是函数调用时（隐式）或外部声明（显式）才会实例化具体的函数。**模板元编程（TMP）可以利用模板代码在编译期间的实例化**，它有以下的优势（注：TMP was an accident; it was discovered, not invented!）

* 编译的代码会变得更小
* 有些代码可以编译期间运行一次，运行时无限使用

```c++
template <unsigned n>
struct Factorial {
    enum { value = n * Factorial<n - 1>::value };
};

template<>		// template class "specialization"
struct Factorial<0> {
    enum { value = 1 };
};

cout << Factorial<10>::value << endl;
```



### 函数指针

------

> **Predicate functions**（谓词函数）: Any function that returns a boolean value is a predicate

```c++
template <typename InputIt, typename UniPred>
int count_occurrences(InputIt begin, InputIt end, UniPred pred) {
    int count = 0;
    for (auto iter = begin; iter != end; ++iter)
        if(pred(*iter)) count++;
    return count;
}

bool isVowel(char c) {
    std::string vowels = "aeiou";
    return vowels.find(c) != std::string::npos;
}

// Usage
std:: string str = "Xadia";
cout << count_occurrences(str.begin(), str.end(), isVowel);
```

在模板函数的类型声明中 `UniPred` 是一个函数指针，这里模板会推断 `UniPred` 是 `bool (*)(char)` 类型



### Lambdas和仿函数

------

> Lambdas are inline, <font color=red>anonymous</font> functions that can know about variables declared in their same scope

> A functor is any class that provides an implementation of operator().

#### 1. 基本lambda语法

* `capture`：捕获列表，主要用于获取外部范围内一些变量，以便在 lambda中使用；
* `params`：参数表（可选），类似于函数参数，无参时可省略；
* `ret`：返回值类型（可选），可以自动推断；
* `{ ... }`：类似函数体；

```c++
auto func1 = [capture](params) -> ret { ... }

// The return-type is optional, if omitted. It's just like an auto return value.
auto func2 = [capture](params) { ... }
```

#### 2. 捕获列表说明

lambda中的捕获列表可以捕获一定范围内的变量

- `[]`：不捕获任何变量
- `[&]`：捕获外部作用域中所有变量，并作为引用在函数体中使用（按引用捕获）。
- `[=]`： 捕获外部作用域中所有变量，并作为副本（<font color=red>只读</font>）在函数体中使用（按值捕获）。
- `[=，&foo]`： 按值捕获外部作用域中所有变量，并按引用捕获 foo 变量。
- `[bar]`：按值捕获 bar 变量，同时不捕获其他变量。
- `[this]`：捕获当前类中的 this 指针，让 lambda 表达式拥有和当前类成员函数同样的访问权限。如果已经使用了 & 或者 =，就默认添加此选项。捕获 this 的目的是可以在 lamda 中使用当前类的成员函数和成员变量。

```c++
void test_lambdas_one() {
    int x = 0, y = 1;
    auto f1 = []  { return x; };				// error
    auto f2 = [&] { return x++; };				// ok
    auto f3 = [=] { return x; };				// ok
    auto f4 = [=] { return x++; };      		// error, read-only 'x'
    auto f5 = [=]() mutable { return ++x; };	// ok, mutable
    // in f5, the "x++" will return 0, but "++x" will return 1.
}
```

#### 3. 延迟调用

```c++
int x = 0;
auto f = [=] { return x; };		// 这里按值捕获，x 的值被复制到f中，后续 x 修改，调用 f 返回的
x += 1;							// 之前的值，也就是最后会输出 0，如果是引用就会输出 1
cout >> f() << endl;
```

#### 4. 注意事项

* 没有捕获变量的 lambda 可以直接转换为函数指针，而有捕获变量的则不行；
* lambdas是套皮的仿函数；
* 在 STL 中有标准函数对象 `std::function<return_type(param_types)>`，lambda，仿函数，函数指针都可以转化成该对象；