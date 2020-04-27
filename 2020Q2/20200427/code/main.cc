#include <iostream>
#include <memory>
#include <vector>

// class Frame {};
// class Frame {
//   public:
//     std::shared_ptr<Frame> GetThis() {
//       return std::shared_ptr<Frame>(this);
//     }
// };
class Frame : public std::enable_shared_from_this<Frame> {
  public:
    std::shared_ptr<Frame> GetThis() {
      return shared_from_this();
    }
};

int main()
{
  // std::shared_ptr<Frame> f(new Frame());
  // // std::shared_ptr<Frame> f1 = new Frame();
  // std::shared_ptr<Frame> f2(f);
  // std::shared_ptr<Frame> f3 = f;
  // f2 = f;
  // std::cout << f3.use_count() << " " << f3.unique() << std::endl;

  // std::shared_ptr<Frame> f4(std::move(new Frame()));
  // // std::shared_ptr<Frame> f5 = std::move(new Frame());
  // std::shared_ptr<Frame> f6(std::move(f4));
  // std::shared_ptr<Frame> f7 = std::move(f6);
  // std::cout << f7.use_count() << " " << f7.unique() << std::endl;

  // // std::shared_ptr<Frame[]> f8(new Frame[10]());
  // std::shared_ptr<Frame> f9(new Frame[10](), std::default_delete<Frame[]>());

  // auto f10 = std::make_shared<Frame>();

  // Frame* f1 = new Frame();
  // std::shared_ptr<Frame> f2(f1);
  // std::shared_ptr<Frame> f3(f1);

  // auto del1 = [](Frame* f){
  //   std::cout << "delete1" << std::endl;
  //   delete f;
  // };
  // auto del2 = [](Frame* f){
  //   std::cout << "delete2" << std::endl;
  //   delete f;
  // };

  // std::shared_ptr<Frame> f1(new Frame(), del1);
  // std::shared_ptr<Frame> f2(new Frame(), del2);

  // std::vector<std::shared_ptr<Frame> > v;
  // v.push_back(f1);
  // v.push_back(f2);

  std::shared_ptr<Frame> f1(new Frame());
  std::shared_ptr<Frame> f2 = f1->GetThis();
  std::cout << f1.use_count() << " " << f2.use_count() << std::endl;

  std::shared_ptr<Frame> f3(new Frame());
  std::shared_ptr<Frame> f4 = f3;
  std::cout << f3.use_count() << " " << f4.use_count() << std::endl;

  return 0;
}

