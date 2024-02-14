#pragma once
#include "sda.h"

namespace srec {

	template <typename T>
	class SecureRecord {

		private:

			sda::SDA<T> data;
			sda::SDA<string> labels;
			int length;

			int getIndex(string label) {
				for (int i = 0; i < length; i++) {
					if (labels.get(i) == label) {
						return i;
					}
				}
				return -1;
			}

		public:

			SecureRecord() {} // not recommended
			SecureRecord(sda::SDA<string> labels_): labels(labels_), length(labels_.getLength()) { data.initialize(labels.getLength()); }

			void reveal() { data.reveal(); labels.reveal(); }
			void hide() { data.hide(); labels.hide(); }

			int getLength() const { return length; }
			string getName(int index) const { return labels.get(index); }

			void set(string label, T value) { int index = getIndex(label); data.set(index, value); }
			T get(string label) { int index = getIndex(label); return data.get(index); }

			string repr() {
				stringstream ss;
				ss << "<";
				for (int i = 0; i < length; i++) {
					ss << labels.get(i) << "=" << data.get(i);
					if (i != length - 1) { ss << ", "; }
				}
				ss << ">";
				return ss.str();
			}

	};

	template <typename T>
	using SR = SecureRecord<T>;

	template <typename T>
	class SecureRecordFormat {

		private:

			sda::SDA<string> names;

		public:

			SecureRecordFormat() {}
			SecureRecordFormat(sda::SDA<string> names): names(names) {}

			SecureRecord<T> make() { return SecureRecord<T>(names); }

			void setNames(sda::SDA<string> names_) { names = names_; }

	};

};
