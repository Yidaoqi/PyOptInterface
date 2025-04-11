#include "pyoptinterface/lp.hpp"
#include "pyoptinterface/lp_impl.hpp"


namespace linear_ip
{
 lp::lp()
{
	ip = new lp_impl();
 }
 lp::lp(const Vector &c, const Matrix &A, const Vector &b)
 {
    ip = new lp_impl(c, A, b);
 }

 lp::~lp()
 {
	 if (ip)
		 delete ip;
 }

 bool lp::is_solved() const{
    return ip->is_solved_;
}

 void lp::solve(){
    ip->solve();
}

 Vector lp::get_x() const{
    assert(is_solved());
    return ip->x_;
}

 Vector lp::get_lam() const{
    assert(is_solved());
    return ip->lam_;
}

 Vector lp::get_s() const{
    assert(is_solved());
    return ip->s_;
}

 Matrix lp::get_A() const{
    return ip->A_;
 }

 void lp::set_A(const std::vector<double> &A, int rows, int cols)
 {
	 Eigen::Map<const Matrix> temp_A(A.data(), rows, cols);
	 ip->A_ = temp_A;
	 ip->rows_ = rows;
	 ip->cols_ = cols;
 }


 Vector lp::get_c() const{
    return ip->c_;
 }

 void lp::set_c(const std::vector<double> &c, size_t length)
 {
	 Eigen::Map<const Vector> temp_c(c.data(), length);
	 ip->c_ = temp_c;
 }

 Vector lp::get_b() const{
    return ip->b_;
 }

 void lp::set_b(const std::vector<double> &b, size_t length)
 {
	 Eigen::Map<const Vector> temp_b(b.data(), length);
	 ip->b_ = temp_b;
 }

 const int lp::get_rows() const
 {
	 return ip->rows_;
 }

 const int lp::get_cols() const
 {
	 return ip->cols_;
 }

 void lp::set_max_itr(const int max_itr){
    assert(max_itr>0);
    ip->max_itr_ = max_itr; 
}

void lp::set_tol(const double tol){
    assert(tol>0);
    ip->tol_ = tol;
}

}
