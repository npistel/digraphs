#include <iostream>
#include <array>
#include <random>
#include <initializer_list>

template <size_t N>
struct Mat
{
	friend std::ostream& operator<<(std::ostream& out, const Mat& mat)
	{
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				out << mat.m[i][j] << ' ';
			}
			out << '\n';
		}

		return out;
	}

	std::size_t n;
	std::array<std::array<bool, N>, N> m;
	
	Mat() : n(N), m() { }

	Mat(std::initializer_list<bool> l) : Mat()
	{
		size_t i = 0;
		for (auto b : l)
		{
			this->m[i / N][i % N] = b;
			i++;
		}
	}

	bool operator==(const Mat& other) const
	{
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				if (this->m[i][j] != other.m[i][j]) return false;
			}
		}

		return true;
	}

	bool operator!=(const Mat& other) const
	{
		return !(*this == other);
	}

	Mat& operator+=(const Mat& other)
	{
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				this->m[i][j] = this->m[i][j] || other.m[i][j];
			}
		}

		return *this;
	}

	Mat operator+(const Mat& other) const
	{
		return Mat(*this) += other;
	}

	Mat operator*(const Mat& other) const
	{
		Mat p;

		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				p.m[i][j] = false;
				for (size_t k = 0; k < N; k++)
				{
					if (this->m[i][k] && other.m[k][j])
					{
						p.m[i][j] = true;
						break;
					}
				}
			}
		}

		return p;
	}

	static Mat random()
	{
		Mat mat;
		std::random_device r;

		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				mat.m[i][j] = r() % 2 == 0;
			}
		}

		return mat;
	}

	static Mat zeros()
	{
		Mat mat;

		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				mat.m[i][j] = false;
			}
		}

		return mat;
	}

	static Mat ones()
	{
		Mat mat;

		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < N; j++)
			{
				mat.m[i][j] = true;
			}
		}

		return mat;
	}

	static Mat eye()
	{
		Mat mat = Mat::zeros();

		for (size_t i = 0; i < N; i++)
		{
			mat.m[i][i] = true;
		}

		return mat;
	}
};

template <size_t N>
void warshall(Mat<N>& a)
{
	for (size_t k = 0; k < a.n; k++)
	{
		for (size_t i = 0; i < a.n; i++)
		{
			if (!a.m[i][k]) continue;
			for (size_t j = 0; j < a.n; j++)
			{
				a.m[i][j] = a.m[i][j] || a.m[k][j];
			}
		}

		std::cout << a << std::endl;
	}
}

template <size_t N>
void lil_warshall(Mat<N>& a)
{
	for (size_t k = 0; k < a.n-1; k++)
	{
		for (size_t i = 0; i < a.n; i++)
		{
			if (!a.m[i][k]) continue;
			for (size_t j = 0; j < a.n; j++)
			{
				a.m[i][j] = a.m[i][j] || a.m[k][j];
			}
		}
	}
}

template <size_t N>
size_t geometric(const Mat<N>& a, const Mat<N>& w)
{
	auto s = Mat<N>::zeros();
	auto p = Mat<N>::eye();

	size_t k = 0;

	while (s != w)
	{
		p = a * p;
		s += p;
		k++;
	}

	return k;
}

template <size_t N>
void test(Mat<N>& a, size_t i = 0)
{
	if (i == N * N)
	{
		auto w = a;
		warshall(w);
		auto k = geometric(a, w);
		bool yup = true;
		for (size_t l = 0; l < N; l++)
		{
			if (a.m[l][l])
			{
				yup = false;
				break;
			}
		}
		auto w2 = a; lil_warshall(w2);
		bool ohmy = true;
		for (size_t l = 0; l < N; l++)
		{
			bool bad = true;
			for (size_t p = 0; p < N; p++)
			{
				if (a.m[l][p] || a.m[p][l])
				{
					bad = false;
					break;
				}
			}

			if (bad)
			{
				ohmy = false;
				break;
			}
		}

		bool yikes = true;
		for (size_t p = 0; p < N; p++)
		{
			for (size_t q = 0; q < N; q++)
			{
				if (a.m[p][q] && a.m[q][p])
				{
					yikes = false;
					break;
				}
			}
		}

		size_t full_rows = 0, empty_rows = 0, full_cols = 0, empty_cols = 0;
		for (size_t p = 0; p < N; p++)
		{
			bool full_row = true, empty_row = true, full_col = true, empty_col = true;
			for (size_t q = 0; q < N; q++)
			{
				if (w.m[p][q]) empty_row = false;
				else full_row = false;

				if (w.m[q][p]) empty_col = false;
				else full_col = false;
			}

			if (full_row) full_rows++;
			else if (empty_row) empty_rows++;

			if (full_col) full_cols++;
			else if (empty_col) empty_cols++;
		}

		size_t ones = 0;
		for (size_t p = 0; p < N; p++)
		{
			for (size_t q = 0; q < N; q++)
			{
				if (a.m[p][q])
				{
					ones++;
				}
			}
		}

		if (ones == 6 && k == N-1 && yikes && ohmy && w != w2 && yup && w != Mat<N>::ones()) std::cout << w << '\n';

		return;
	}

	a.m[i / N][i % N] = 0;
	test(a, i + 1);
	a.m[i / N][i % N] = 1;
	test(a, i + 1);

	a.m[i / N][i % N] = 0;
}

int main()
{
	Mat<4> m = {
		0, 0, 0, 1,
		1, 0, 1, 1,
		1, 0, 0, 0,
		0, 0, 1, 0
	};

	warshall(m);

	return 0;

	std::cout << "test\n";
	auto a = Mat<4>();
	test(a);

	return 0;
}