### 异常处理😁

------

#### 1. assert

`assert`是 C 标准库中的一个宏（<assert.h>），如果在编译选项中传入 `NDEBUG` 这一宏定义会取消掉所有的断言

```c
#ifdef NDEBUG
# define assert(condition) ((void)0)
#else
# define assert(condition) /*implementation defined*/
#endif
```

#### 2. 异常处理

```c++
float ratio(float a, float b) {
    if (a < 0) 
        throw 1;
    if (fabs(a + b) < FLT_EPSILON)
    	throw "Error ...";
    return (a - b) / (a + b);
}

try {
    z = ratio(x, y);				// ratio函数内抛出异常会被这里catch捕获
} catch (const char* msg) { 		// [注]：如果这里没有可以接住的catch块，这些异常
    // ... 						    //      会继续它的调用者抛出，直到能main
} catch (int eid) {	
    // ... 
}
```

如果一个异常没有被捕获（catch）,它将会一直到达顶层的调用者，同时**终止程序**。这里常常在顶层设置一个可以捕获所有异常的块

```c++
int main() {
	try {
      	// some operation.  		
    } catch(...) {
        std::err << "Unrecognized Exception" << std::endl;
    }
}
```

也可以抛出（throw）一个对象，如下例子和**顺序**有很大关系，以下调换顺序可以被 `Derived` 的块捕获异常

```c++
try {
	throw Derived();
} catch (const Base& base) {
	std::cerr << "Base" << std::endl;
} catch (const Derived& derived) {
	std::cerr << "Derived" << std::endl;		// 用于不会到达这里
}
```

#### 3. 其他

* 指定函数步抛出异常（<font color=red>`noexcept`</font>）：`void func() noexcept;`
* 不抛出异常的`new`（<font color=red>`nothrow`</font>）: `auto p = new(nothrow) int[10];`



### RAII🙃

------

RAII（**R**esource **A**cquisition **I**s **I**nitialization），可以理解为在构造函数中申请资源，而在析构函数中释放，也有说法是CADRE（**C**onstructor **A**cquires, **D**estructor **R**eleases）。

* 对象不应该存在半有效的状态，一旦创建，就应该可以使用；
* 析构函数总是应该被调用，即使是在发生异常的情况下，确保资源肯定会被被释放；

#### 1. 一个文件的例子：

```c++
void printFile () {
    ifstream input();				 // 创建流对象，但是没完全创建
    input.open(“hamlet.txt”);		 // 还需要指向特定文件
    string line;
    while (getline(input, line)) {   // 可能会抛出异常
    	cout << line << endl;
    }
    input.close();					 // 关闭了，但是没有在析构函数关闭，不RAII的精神
}
```

```c++
void printFile () {
    ifstream input(“hamlet.txt”);	 // 一次完成	
    string line;
    while (getline(input, line)) {   // 可能会抛出异常
    	cout << line << endl;
    }
} 									 // 流调用析构函数，释放资源
```

#### 2. 一个关于锁的例子

```c++
void cleanDatabase (mutex& databaseLock,
					map<int, int>& database) {
    databaseLock.lock();
    // other threads will not modify database
    // modify the database
    // if exception thrown, mutex never unlocked!
    databaseLock.unlock();
}
```

这是标准的 `C` 风格代码的写法，不符合 `RAII` 的精神，改进代码如下

```c++
void cleanDatabase (mutex& databaseLock,
					map<int, int>& database) {
    lock_guard<mutex>(databaseLock);
    // other threads will not modify database
    // modify the database
    // if exception thrown, that’s fine!
	// no release call needed
}   // lock always unlocked when function exits. 
```

关于 `lock_guard` 的实现，就是把获取锁和释放锁封装在构造和析构函数里

```c++
class lock_guard {
public:
    lock_guard(mutex& lock) : acquired_lock(lock) {	// 使用初始化列表，而非复制和移动操作
    	acquired_lock.lock()
    }
    ~lock_guard() {
    	acquired_lock.unlock();
    }
private:
    mutex& acquired_lock;
}
```

对于`mutex` 的实现，不需要使用拷贝和移动操作都去除（Why?）

```c++
class mutex {
public:
    mutex(const mutex& other) = delete;
    mutex& operator=(const mutex& rhs) = delete;
    mutex(mutex&& other) = delete;
    mutex& operator=(mutex&& other) = delete;
}
```



### 智能指针😎

------

上面的例子可以将申请资源和释放资源的操作放在构造函数和析构函数，但是内置的类型却不能实现这样的操作，例如原生的指针。

```c++
void rawPtrFn () {
	Node* n = new Node;
	// do some stuff with n…
    // if exception thrown, n never deleted.
	delete n;
}
```

既然原生的类型无法实现，那就包装一下，这里把指针包装成类，包括以下几种类型

* `STL` : `unique_ptr`，`shared_ptr`，`weak_ptr`，`auto_ptr`（弃用）

* `boost` ：`scoped_ptr`，`intrusive_ptr`

#### 1. unique_ptr

唯一拥有指向的资源，一旦删除，索引的资源也就被销毁。所以该指针<font color=red>**不能被复制**</font>

```c++
void rawPtrFn () {
    std::unique_ptr<Node> n(new Node);
    // do some stuff with n
} // Freed!
```

如果发生复制，会有多个指针指向同一块资源，这样不同的`unique_ptr`析构函数会多次释放同一块资源

<img src="asset\uptr_copy.PNG" style="zoom:75%;" />

在实现上需要将复制和移动操作都删除掉，一个比较糙的实现（<font color=#008000>是否需要也禁用掉移动操作</font>）

```c++
template<typename T>
class unique_ptr {
public:
    ~unique_ptr() { delete ptr;}
    unique_ptr(const unique_ptr& other) = delete;
    unique_ptr& operator=(const unique_ptr& rhs) = delete;
    unique_ptr(unique_ptr&& other) = delete;
    unique_ptr& operator=(unique_ptr&& other) = delete;
    // other operation
private:
    T* ptr;
}
```

#### 2. shared_ptr

如果想多个指针指向同一块资源，用这个`shared_ptr`，只有在没有共享指针指向这块资源时才会被释放

```c++
{
    std::shared_ptr<int> p1(new int);
    // Use p1
    {
        std::shared_ptr<int> p2 = p1;		// 必须支持复制操作
        // Use p1 and p2
    }
    // Use p1
}
// Freed!
```

这里应该有一个资源的引用计数，用来记录有多少个共享指针指向该资源

* 在拷贝构造函数和拷贝复制操作自增；

* 在析构或<font color=red>拷贝被覆盖</font>时候递减；

* 在引用计数为 `0` 释放该资源；

```c++
template<typename T>
class shared_ptr {
public:
    // other opreation.
private:
    size_t* _ref_count;			// 这里指向引用计数，这里会有数据竞争的问题
    T* _ptr;					// 原生指针，用来指向资源位置
}
```

#### 3. 如何创建智能指针

<img src="asset\make_smart-ptr.PNG" style="zoom:65%;" />

```
std::unique_ptr<Node> n = std::make_unique<Node>();
std::shared_ptr<Node> n(new Node);
```

**在现代 `C++` 中，应该尽可能的避免直接使用 `new` 和 `delete` 操作，这样减少很多内存泄漏**