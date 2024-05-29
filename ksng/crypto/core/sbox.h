#pragma once
#include "../../util/data.h"

namespace {

	class SubstitutionBox {

		private:

			data::Bytes fbox;
			data::Bytes bbox;

			SubstitutionBox(data::Bytes box): fbox(box), bbox(data::Bytes(256)) {
				if (fbox.getLength() != 256) {
					notif::fatal("substitution: incorrect sbox size (must be 256)");
				}
				for (int i = 0; i < 256; i++) {
					for (int j = 0; j < 256; j++) {
						if (fbox.get(j) == i) {
							bbox.set(i, fbox.get(j));
						}
					}
				}
			}

			data::Bytes sub(data::Bytes pt) {
				data::Bytes ct(pt.getLength());
				for (int i = 0; i < pt.getLength(); i++) { ct.set(i, fbox.get(pt.get(i))); }
				return ct;
			}

			data::Bytes des(data::Bytes ct) {
				data::Bytes pt(ct.getLength());
				for (int i = 0; i < ct.getLength(); i++) { pt.set(i, bbox.get(ct.get(i))); }
				return pt;
			}

	};

};