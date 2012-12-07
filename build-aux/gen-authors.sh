#! /bin/sh

echo '# Copyright (c) 2004-2012 Nicta, OML Contributors <oml-user@lists.nicta.com.au>'
echo '#'
echo '# Licensing information can be found in file COPYING.'
echo '#'
echo '# Specific terms apply to Iperf and can be found in file iperf/COPYING.'
echo ''

(
  cat iperf/AUTHORS | sed -n -e 's/^[^ ].*/& (Upstream Iperf)/p'
  (git log; cd iperf; git log) | sed -n -e 's/^Author: *//p'
) \
  | sed -e 's/ *<.*>//' \
	-e 's/jdugan/John Dugan (Upstream Iperf)/' \
	-e 's/jestabro/John Estabrook (Upstream Iperf)/' \
	-e 's/Olivier Mehani.*/Olivier Mehani/'\
	-e 's/gjourjon/Guillaume Jourjon/'\
	-e 's/qosx/Guillaume Jourjon/' \
	-e 's/root/Guillaume Jourjon/' \
  | sort \
  | uniq
