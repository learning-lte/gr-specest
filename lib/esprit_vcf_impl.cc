/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Communications Engineering Lab, KIT
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "esprit_vcf_impl.h"
#include <gnuradio/io_signature.h>
#include <specest/esprit_fortran_algo.h>

namespace gr {
namespace specest {

esprit_vcf::sptr esprit_vcf::make(unsigned int n, unsigned int m, unsigned int nsamples)
{
    return gnuradio::get_initial_sptr(new esprit_vcf_impl(n, m, nsamples));
}

esprit_vcf_impl::esprit_vcf_impl(unsigned int n, unsigned int m, unsigned int nsamples)
    : gr::sync_block("esprit_vcf",
                     gr::io_signature::make(1, 1, nsamples * sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, n * sizeof(float))),
      d_n(n),
      d_m(m),
      d_nsamples(nsamples),
      d_algo(new esprit_fortran_algo(n, m)),
      d_in_buf(d_nsamples, 0),
      d_out_buf(d_n, 0)
{
}

esprit_vcf_impl::~esprit_vcf_impl() { delete d_algo; }

int esprit_vcf_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const gr_complex* in = static_cast<const gr_complex*>(input_items[0]);
    float* out = static_cast<float*>(output_items[0]);

    for (int item = 0; item < noutput_items; item++) {
        for (int i = 0; i < d_nsamples; i++)
            d_in_buf[i] = static_cast<gr_complexd>(in[i]);

        d_algo->calculate(&d_in_buf[0], d_nsamples, &d_out_buf[0]);

        for (int i = 0; i < d_n; i++)
            out[i] = float(d_out_buf[i]);

        in += d_nsamples;
        out += d_n;
    }

    return noutput_items;
}

} /* namespace specest */
} /* namespace gr */
