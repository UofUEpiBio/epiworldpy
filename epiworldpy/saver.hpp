#ifndef EPIWORLDPY_SAVER_HPP
#define EPIWORLDPY_SAVER_HPP

#include "common.hpp"
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
	auto out(std::ostream &stream) const -> const std::ostream &;

	void set_cvsloc(std::string cvsloc);
	[[nodiscard]] auto get_cvsloc() const -> const std::string_view;

	auto operator*() -> std::function<void(size_t, epiworld::Model<int> *)>;
};

void export_saver(pybind11::class_<Saver, std::shared_ptr<Saver>> &c);
} // namespace epiworldpy

#endif /* EPIWORLDPY_SAVER_HPP */
