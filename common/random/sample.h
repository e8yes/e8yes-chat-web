/**
 * e8yes demo web.
 *
 * <p>Copyright (C) 2020 Chifeng Wen {daviesx66@gmail.com}
 *
 * <p>This program is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * <p>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * <p>You should have received a copy of the GNU General Public License along with this program. If
 * not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SAMPLE_H
#define SAMPLE_H

#include <cassert>
#include <cmath>
#include <unordered_map>
#include <vector>

#include "common/random/random_source.h"

namespace e8 {

/**
 * @brief SampleFrom Sample a key from the specified discrete distribution with the key's
 * probability.
 */
template <typename KeyType>
KeyType SampleFrom(std::unordered_map<KeyType, float> const &discrete_distri,
                   RandomSource *random_source) {
    double q = random_source->Draw();
    double cdf = 0;

    KeyType last_key;
    for (auto const &[key, p] : discrete_distri) {
        cdf += p;
        if (q < cdf) {
            return key;
        }

        last_key = key;
    }

    assert(std::abs(cdf - 1.0f) < 1e-2f);

    // Numerical glitch.
    return last_key;
}

} // namespace e8

#endif // SAMPLE_H
