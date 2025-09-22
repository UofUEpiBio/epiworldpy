#ifndef EPIWORLDPY_SAVER_HPP
#define EPIWORLDPY_SAVER_HPP

#include "epiworld-common.hpp"
#include <pybind11/pybind11.h>

namespace epiworldpy {
class Saver {
  private:
	std::function<void(size_t, epiworld::Model<int> *)> fun;
	std::vector<std::string> what;
	std::string csvloc;
	std::string fn;
	std::string id;
	bool file_output;

  public:
	Saver(std::vector<std::string> what, std::string fn, std::string id,
		  bool file_output);

	void unlink_siblings() const;
	const std::ostream &out(std::ostream &stream) const;

	void set_cvsloc(std::string cvsloc);
	const std::string_view get_cvsloc() const;

	std::function<void(size_t, epiworld::Model<int> *)> operator*();
};

void export_saver(pybind11::class_<Saver, std::shared_ptr<Saver>> &c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_SAVER_HPP */
