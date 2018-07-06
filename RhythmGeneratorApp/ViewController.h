#pragma once

#include <poScene/ViewController.h>

typedef std::shared_ptr<class ViewController> ViewControllerRef;


class ViewController : public po::scene::ViewController 
{
public:
  void viewDidLoad() override;
  void cleanup();

protected:
  ViewController() = default;

private:
  
};
