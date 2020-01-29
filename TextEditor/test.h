template <typename It>
struct IteratorRange {
	It first, last;

	IteratorRange(It begin, It end)
			: first(begin)
			, last(end)
	{
	}
};

template<typename T>
auto MakeRange(T& container) {
	return IteratorRange(container.begin(), container.end());
}


void foo()
{
	IteratorRange  r(1,23);}