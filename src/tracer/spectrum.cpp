#include "tracer/spectrum.hpp"

namespace tracer {

  bool nspectrum::is_black() const {
    for (size_t i = 0; i < n_samples; ++i) {
      if (!COMPARE_EQ(spd[i], 0)) return false;
    }
    return true;
  }

  nspectrum nspectrum::sqrt() const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] = std::sqrt(spd[i]);
    }
    return result;
  }

  nspectrum nspectrum::pow(Float x) const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] = std::pow(spd[i], x);
    }
    return result;
  }

  nspectrum nspectrum::exp() const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] = std::exp(spd[i]);
    }
    return result;
  }

  nspectrum nspectrum::clamp(Float min, Float max) const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] = math::clamp(spd[i], min, max);
    }
    return result;
  }

  nspectrum nspectrum::inverse() const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      ASSERT(!COMPARE_EQ(spd[i], 0));
      result.spd[i] = 1.f / spd[i];
    }
    return result;
  }

  Float nspectrum::average() const {
    Float sum = 0;
    for (size_t i = 0; i < n_samples; ++i) {
      sum += spd[i];
    }
    sum /= n_samples;
    return sum;
  }

  nspectrum& nspectrum::operator=(const nspectrum& sp) {
    for (size_t i = 0; i < n_samples; ++i) {
      spd[i] = sp.spd[i];
    }
    return *this;
  }

  nspectrum nspectrum::operator+(const nspectrum& sp) const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] = spd[i] + sp.spd[i];
    }
    return result;
  }

  nspectrum nspectrum::operator-(const nspectrum& sp) const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] = spd[i] - sp.spd[i];
    }
    return result;
  }

  nspectrum nspectrum::operator*(const nspectrum& sp) const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] = spd[i] * sp.spd[i];
    }
    return result;
  }

  nspectrum nspectrum::operator/(const nspectrum& sp) const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      ASSERT(!COMPARE_EQ(sp.spd[i], 0));
      result.spd[i] = spd[i] / sp.spd[i];
    }
    return result;
  }

  nspectrum nspectrum::operator*(Float s) const {
    nspectrum result(*this);
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] *= s;
    }
    return result;
  }

  nspectrum nspectrum::operator/(Float s) const {
    nspectrum result(*this);
    ASSERT(!COMPARE_EQ(s, 0));
    Float inv = 1 / s;
    for (size_t i = 0; i < n_samples; ++i) {
      result.spd[i] *= inv;
    }
    return result;
  }

  nspectrum& nspectrum::operator+=(const nspectrum& sp) {
    for (size_t i = 0; i < n_samples; ++i) {
      spd[i] += sp.spd[i];
    }
    return *this;
  }

  nspectrum& nspectrum::operator-=(const nspectrum& sp) {
    for (size_t i = 0; i < n_samples; ++i) {
      spd[i] -= sp.spd[i];
    }
    return *this;
  }

  nspectrum& nspectrum::operator*=(const nspectrum& sp) {
    for (size_t i = 0; i < n_samples; ++i) {
      spd[i] *= sp.spd[i];
    }
    return *this;
  }

  nspectrum& nspectrum::operator/=(const nspectrum& sp) {
    for (size_t i = 0; i < n_samples; ++i) {
      ASSERT(!COMPARE_EQ(sp.spd[i], 0));
      spd[i] /= sp.spd[i];
    }
    return *this;
  }

  sampled_spectrum::sampled_spectrum(Float v) : nspectrum(60, v) {}

  sampled_spectrum::sampled_spectrum(std::vector<spectral_sample> samples) : nspectrum(60) {
    std::sort(
        samples.begin(),
        samples.end(),
        [](const spectral_sample& a, const spectral_sample& b) -> bool {
        return a.lambda < b.lambda;
        }
        );
    for (size_t i = 0; i < this->n_samples; ++i) {
      Float lambda0 = math::lerp(Float(i) / this->n_samples, LAMBDA_START, LAMBDA_END);
      Float lambda1 = math::lerp(Float(i+1) / this->n_samples, LAMBDA_START, LAMBDA_END);
      spd[i] = average_spectral_samples(samples, lambda0, lambda1);
    }
  }

  math::vector3f sampled_spectrum::xyz() const {
    math::vector3f spectrum(0.0);
    for (int i = 0; i < N_SPECTRAL_SAMPLES; ++i) {
      spectrum.x += spd[i] * SMC_X.spd[i];
      spectrum.y += spd[i] * SMC_Y.spd[i];
      spectrum.z += spd[i] * SMC_Z.spd[i];
    }
    spectrum *= Float(LAMBDA_END - LAMBDA_START) / N_SPECTRAL_SAMPLES;
    return spectrum;
  }

  Float sampled_spectrum::average_spectral_samples(
      const std::vector<spectral_sample>& samples,
      const Float lambda0,
      const Float lambda1
      )
  {
    if (samples.empty()) return 0;
    if (samples.size() == 1) return samples.front().value;
    if (lambda0 >= samples.back().lambda) return samples.back().value;
    if (lambda1 <= samples.front().lambda) return samples.front().value;

    Float sum = 0;
    if (lambda0 < samples.front().lambda)
      sum += samples.front().value * (samples.front().lambda - lambda0);
    if (lambda1 > samples.back().lambda)
      sum += samples.back().value * (lambda1 - samples.back().lambda);

    size_t i = 0;
    while (i < samples.size() && samples[i+1].lambda < lambda0) ++i;

    auto interp = [&](Float lambda, size_t i) -> Float {
      return math::lerp(
          (lambda - samples[i].lambda) / (samples[i+1].lambda - samples[i].lambda),
          samples[i].value,
          samples[i+1].value
          );
    };

    for (; i+1 < samples.size() && samples[i].lambda <= lambda1; ++i) {
      Float lambda_l = std::max(samples[i].lambda, lambda0);
      Float lambda_r = std::min(samples[i+1].lambda, lambda1);
      sum += 0.5 * (interp(lambda_l, i) + interp(lambda_r, i)) * (lambda_r - lambda_l);
    }

    return sum / (lambda1 - lambda0);
  }

  std::vector<spectral_sample> sampled_spectrum::create_spectral_samples(
      const std::vector<Float>& lambdas,
      const std::vector<Float>& values
      )
  {
    std::vector<spectral_sample> ret(lambdas.size());
    for (size_t i = 0; i < lambdas.size(); ++i) {
      ret[i] = { lambdas[i], values[i] };
    }
    return ret;
  }

  const std::vector<Float> CIE_LAMBDAS {
    360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374,
    375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389,
    390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404,
    405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419,
    420, 421, 422, 423, 424, 425, 426, 427, 428, 429, 430, 431, 432, 433, 434,
    435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449,
    450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464,
    465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479,
    480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494,
    495, 496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509,
    510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524,
    525, 526, 527, 528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539,
    540, 541, 542, 543, 544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554,
    555, 556, 557, 558, 559, 560, 561, 562, 563, 564, 565, 566, 567, 568, 569,
    570, 571, 572, 573, 574, 575, 576, 577, 578, 579, 580, 581, 582, 583, 584,
    585, 586, 587, 588, 589, 590, 591, 592, 593, 594, 595, 596, 597, 598, 599,
    600, 601, 602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 612, 613, 614,
    615, 616, 617, 618, 619, 620, 621, 622, 623, 624, 625, 626, 627, 628, 629,
    630, 631, 632, 633, 634, 635, 636, 637, 638, 639, 640, 641, 642, 643, 644,
    645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655, 656, 657, 658, 659,
    660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673, 674,
    675, 676, 677, 678, 679, 680, 681, 682, 683, 684, 685, 686, 687, 688, 689,
    690, 691, 692, 693, 694, 695, 696, 697, 698, 699, 700, 701, 702, 703, 704,
    705, 706, 707, 708, 709, 710, 711, 712, 713, 714, 715, 716, 717, 718, 719,
    720, 721, 722, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 734,
    735, 736, 737, 738, 739, 740, 741, 742, 743, 744, 745, 746, 747, 748, 749,
    750, 751, 752, 753, 754, 755, 756, 757, 758, 759, 760, 761, 762, 763, 764,
    765, 766, 767, 768, 769, 770, 771, 772, 773, 774, 775, 776, 777, 778, 779,
    780, 781, 782, 783, 784, 785, 786, 787, 788, 789, 790, 791, 792, 793, 794,
    795, 796, 797, 798, 799, 800, 801, 802, 803, 804, 805, 806, 807, 808, 809,
    810, 811, 812, 813, 814, 815, 816, 817, 818, 819, 820, 821, 822, 823, 824,
    825, 826, 827, 828, 829, 830
  };
  const std::vector<spectral_sample> CIE_X(
      sampled_spectrum::create_spectral_samples(CIE_LAMBDAS, {
        0.0001299000f,   0.0001458470f,   0.0001638021f,   0.0001840037f,
        0.0002066902f,   0.0002321000f,   0.0002607280f,   0.0002930750f,
        0.0003293880f,   0.0003699140f,   0.0004149000f,   0.0004641587f,
        0.0005189860f,   0.0005818540f,   0.0006552347f,   0.0007416000f,
        0.0008450296f,   0.0009645268f,   0.001094949f,    0.001231154f,
        0.001368000f,    0.001502050f,    0.001642328f,    0.001802382f,
        0.001995757f,    0.002236000f,    0.002535385f,    0.002892603f,
        0.003300829f,    0.003753236f,    0.004243000f,    0.004762389f,
        0.005330048f,    0.005978712f,    0.006741117f,    0.007650000f,
        0.008751373f,    0.01002888f,     0.01142170f,     0.01286901f,
        0.01431000f,     0.01570443f,     0.01714744f,     0.01878122f,
        0.02074801f,     0.02319000f,     0.02620736f,     0.02978248f,
        0.03388092f,     0.03846824f,     0.04351000f,     0.04899560f,
        0.05502260f,     0.06171880f,     0.06921200f,     0.07763000f,
        0.08695811f,     0.09717672f,     0.1084063f,      0.1207672f,
        0.1343800f,      0.1493582f,      0.1653957f,      0.1819831f,
        0.1986110f,      0.2147700f,      0.2301868f,      0.2448797f,
        0.2587773f,      0.2718079f,      0.2839000f,      0.2949438f,
        0.3048965f,      0.3137873f,      0.3216454f,      0.3285000f,
        0.3343513f,      0.3392101f,      0.3431213f,      0.3461296f,
        0.3482800f,      0.3495999f,      0.3501474f,      0.3500130f,
        0.3492870f,      0.3480600f,      0.3463733f,      0.3442624f,
        0.3418088f,      0.3390941f,      0.3362000f,      0.3331977f,
        0.3300411f,      0.3266357f,      0.3228868f,      0.3187000f,
        0.3140251f,      0.3088840f,      0.3032904f,      0.2972579f,
        0.2908000f,      0.2839701f,      0.2767214f,      0.2689178f,
        0.2604227f,      0.2511000f,      0.2408475f,      0.2298512f,
        0.2184072f,      0.2068115f,      0.1953600f,      0.1842136f,
        0.1733273f,      0.1626881f,      0.1522833f,      0.1421000f,
        0.1321786f,      0.1225696f,      0.1132752f,      0.1042979f,
        0.09564000f,     0.08729955f,     0.07930804f,     0.07171776f,
        0.06458099f,     0.05795001f,     0.05186211f,     0.04628152f,
        0.04115088f,     0.03641283f,     0.03201000f,     0.02791720f,
        0.02414440f,     0.02068700f,     0.01754040f,     0.01470000f,
        0.01216179f,     0.009919960f,    0.007967240f,    0.006296346f,
        0.004900000f,    0.003777173f,    0.002945320f,    0.002424880f,
        0.002236293f,    0.002400000f,    0.002925520f,    0.003836560f,
        0.005174840f,    0.006982080f,    0.009300000f,    0.01214949f,
        0.01553588f,     0.01947752f,     0.02399277f,     0.02910000f,
        0.03481485f,     0.04112016f,     0.04798504f,     0.05537861f,
        0.06327000f,     0.07163501f,     0.08046224f,     0.08973996f,
        0.09945645f,     0.1096000f,      0.1201674f,      0.1311145f,
        0.1423679f,      0.1538542f,      0.1655000f,      0.1772571f,
        0.1891400f,      0.2011694f,      0.2133658f,      0.2257499f,
        0.2383209f,      0.2510668f,      0.2639922f,      0.2771017f,
        0.2904000f,      0.3038912f,      0.3175726f,      0.3314384f,
        0.3454828f,      0.3597000f,      0.3740839f,      0.3886396f,
        0.4033784f,      0.4183115f,      0.4334499f,      0.4487953f,
        0.4643360f,      0.4800640f,      0.4959713f,      0.5120501f,
        0.5282959f,      0.5446916f,      0.5612094f,      0.5778215f,
        0.5945000f,      0.6112209f,      0.6279758f,      0.6447602f,
        0.6615697f,      0.6784000f,      0.6952392f,      0.7120586f,
        0.7288284f,      0.7455188f,      0.7621000f,      0.7785432f,
        0.7948256f,      0.8109264f,      0.8268248f,      0.8425000f,
        0.8579325f,      0.8730816f,      0.8878944f,      0.9023181f,
        0.9163000f,      0.9297995f,      0.9427984f,      0.9552776f,
        0.9672179f,      0.9786000f,      0.9893856f,      0.9995488f,
        1.0090892f,      1.0180064f,      1.0263000f,      1.0339827f,
        1.0409860f,      1.0471880f,      1.0524667f,      1.0567000f,
        1.0597944f,      1.0617992f,      1.0628068f,      1.0629096f,
        1.0622000f,      1.0607352f,      1.0584436f,      1.0552244f,
        1.0509768f,      1.0456000f,      1.0390369f,      1.0313608f,
        1.0226662f,      1.0130477f,      1.0026000f,      0.9913675f,
        0.9793314f,      0.9664916f,      0.9528479f,      0.9384000f,
        0.9231940f,      0.9072440f,      0.8905020f,      0.8729200f,
        0.8544499f,      0.8350840f,      0.8149460f,      0.7941860f,
        0.7729540f,      0.7514000f,      0.7295836f,      0.7075888f,
        0.6856022f,      0.6638104f,      0.6424000f,      0.6215149f,
        0.6011138f,      0.5811052f,      0.5613977f,      0.5419000f,
        0.5225995f,      0.5035464f,      0.4847436f,      0.4661939f,
        0.4479000f,      0.4298613f,      0.4120980f,      0.3946440f,
        0.3775333f,      0.3608000f,      0.3444563f,      0.3285168f,
        0.3130192f,      0.2980011f,      0.2835000f,      0.2695448f,
        0.2561184f,      0.2431896f,      0.2307272f,      0.2187000f,
        0.2070971f,      0.1959232f,      0.1851708f,      0.1748323f,
        0.1649000f,      0.1553667f,      0.1462300f,      0.1374900f,
        0.1291467f,      0.1212000f,      0.1136397f,      0.1064650f,
        0.09969044f,     0.09333061f,     0.08740000f,     0.08190096f,
        0.07680428f,     0.07207712f,     0.06768664f,     0.06360000f,
        0.05980685f,     0.05628216f,     0.05297104f,     0.04981861f,
        0.04677000f,     0.04378405f,     0.04087536f,     0.03807264f,
        0.03540461f,     0.03290000f,     0.03056419f,     0.02838056f,
        0.02634484f,     0.02445275f,     0.02270000f,     0.02108429f,
        0.01959988f,     0.01823732f,     0.01698717f,     0.01584000f,
        0.01479064f,     0.01383132f,     0.01294868f,     0.01212920f,
        0.01135916f,     0.01062935f,     0.009938846f,    0.009288422f,
        0.008678854f,    0.008110916f,    0.007582388f,    0.007088746f,
        0.006627313f,    0.006195408f,    0.005790346f,    0.005409826f,
        0.005052583f,    0.004717512f,    0.004403507f,    0.004109457f,
        0.003833913f,    0.003575748f,    0.003334342f,    0.003109075f,
        0.002899327f,    0.002704348f,    0.002523020f,    0.002354168f,
        0.002196616f,    0.002049190f,    0.001910960f,    0.001781438f,
        0.001660110f,    0.001546459f,    0.001439971f,    0.001340042f,
        0.001246275f,    0.001158471f,    0.001076430f,    0.0009999493f,
        0.0009287358f,   0.0008624332f,   0.0008007503f,   0.0007433960f,
        0.0006900786f,   0.0006405156f,   0.0005945021f,   0.0005518646f,
        0.0005124290f,   0.0004760213f,   0.0004424536f,   0.0004115117f,
        0.0003829814f,   0.0003566491f,   0.0003323011f,   0.0003097586f,
        0.0002888871f,   0.0002695394f,   0.0002515682f,   0.0002348261f,
        0.0002191710f,   0.0002045258f,   0.0001908405f,   0.0001780654f,
        0.0001661505f,   0.0001550236f,   0.0001446219f,   0.0001349098f,
        0.0001258520f,   0.0001174130f,   0.0001095515f,   0.0001022245f,
        0.00009539445f,  0.00008902390f,  0.00008307527f,  0.00007751269f,
        0.00007231304f,  0.00006745778f,  0.00006292844f,  0.00005870652f,
        0.00005477028f,  0.00005109918f,  0.00004767654f,  0.00004448567f,
        0.00004150994f,  0.00003873324f,  0.00003614203f,  0.00003372352f,
        0.00003146487f,  0.00002935326f,  0.00002737573f,  0.00002552433f,
        0.00002379376f,  0.00002217870f,  0.00002067383f,  0.00001927226f,
        0.00001796640f,  0.00001674991f,  0.00001561648f,  0.00001455977f,
        0.00001357387f,  0.00001265436f,  0.00001179723f,  0.00001099844f,
        0.00001025398f,  0.000009559646f, 0.000008912044f, 0.000008308358f,
        0.000007745769f, 0.000007221456f, 0.000006732475f, 0.000006276423f,
        0.000005851304f, 0.000005455118f, 0.000005085868f, 0.000004741466f,
        0.000004420236f, 0.000004120783f, 0.000003841716f, 0.000003581652f,
        0.000003339127f, 0.000003112949f, 0.000002902121f, 0.000002705645f,
        0.000002522525f, 0.000002351726f, 0.000002192415f, 0.000002043902f,
        0.000001905497f, 0.000001776509f, 0.000001656215f, 0.000001544022f,
        0.000001439440f, 0.000001341977f, 0.000001251141f
      })
  );
  const std::vector<spectral_sample> CIE_Y(
      sampled_spectrum::create_spectral_samples(CIE_LAMBDAS, {
        0.000003917000f,  0.000004393581f,  0.000004929604f,  0.000005532136f,
        0.000006208245f,  0.000006965000f,  0.000007813219f,  0.000008767336f,
        0.000009839844f,  0.00001104323f,   0.00001239000f,   0.00001388641f,
        0.00001555728f,   0.00001744296f,   0.00001958375f,   0.00002202000f,
        0.00002483965f,   0.00002804126f,   0.00003153104f,   0.00003521521f,
        0.00003900000f,   0.00004282640f,   0.00004691460f,   0.00005158960f,
        0.00005717640f,   0.00006400000f,   0.00007234421f,   0.00008221224f,
        0.00009350816f,   0.0001061361f,    0.0001200000f,    0.0001349840f,
        0.0001514920f,    0.0001702080f,    0.0001918160f,    0.0002170000f,
        0.0002469067f,    0.0002812400f,    0.0003185200f,    0.0003572667f,
        0.0003960000f,    0.0004337147f,    0.0004730240f,    0.0005178760f,
        0.0005722187f,    0.0006400000f,    0.0007245600f,    0.0008255000f,
        0.0009411600f,    0.001069880f,     0.001210000f,     0.001362091f,
        0.001530752f,     0.001720368f,     0.001935323f,     0.002180000f,
        0.002454800f,     0.002764000f,     0.003117800f,     0.003526400f,
        0.004000000f,     0.004546240f,     0.005159320f,     0.005829280f,
        0.006546160f,     0.007300000f,     0.008086507f,     0.008908720f,
        0.009767680f,     0.01066443f,      0.01160000f,      0.01257317f,
        0.01358272f,      0.01462968f,      0.01571509f,      0.01684000f,
        0.01800736f,      0.01921448f,      0.02045392f,      0.02171824f,
        0.02300000f,      0.02429461f,      0.02561024f,      0.02695857f,
        0.02835125f,      0.02980000f,      0.03131083f,      0.03288368f,
        0.03452112f,      0.03622571f,      0.03800000f,      0.03984667f,
        0.04176800f,      0.04376600f,      0.04584267f,      0.04800000f,
        0.05024368f,      0.05257304f,      0.05498056f,      0.05745872f,
        0.06000000f,      0.06260197f,      0.06527752f,      0.06804208f,
        0.07091109f,      0.07390000f,      0.07701600f,      0.08026640f,
        0.08366680f,      0.08723280f,      0.09098000f,      0.09491755f,
        0.09904584f,      0.1033674f,       0.1078846f,       0.1126000f,
        0.1175320f,       0.1226744f,       0.1279928f,       0.1334528f,
        0.1390200f,       0.1446764f,       0.1504693f,       0.1564619f,
        0.1627177f,       0.1693000f,       0.1762431f,       0.1835581f,
        0.1912735f,       0.1994180f,       0.2080200f,       0.2171199f,
        0.2267345f,       0.2368571f,       0.2474812f,       0.2586000f,
        0.2701849f,       0.2822939f,       0.2950505f,       0.3085780f,
        0.3230000f,       0.3384021f,       0.3546858f,       0.3716986f,
        0.3892875f,       0.4073000f,       0.4256299f,       0.4443096f,
        0.4633944f,       0.4829395f,       0.5030000f,       0.5235693f,
        0.5445120f,       0.5656900f,       0.5869653f,       0.6082000f,
        0.6293456f,       0.6503068f,       0.6708752f,       0.6908424f,
        0.7100000f,       0.7281852f,       0.7454636f,       0.7619694f,
        0.7778368f,       0.7932000f,       0.8081104f,       0.8224962f,
        0.8363068f,       0.8494916f,       0.8620000f,       0.8738108f,
        0.8849624f,       0.8954936f,       0.9054432f,       0.9148501f,
        0.9237348f,       0.9320924f,       0.9399226f,       0.9472252f,
        0.9540000f,       0.9602561f,       0.9660074f,       0.9712606f,
        0.9760225f,       0.9803000f,       0.9840924f,       0.9874812f,
        0.9903128f,       0.9928116f,       0.9949501f,       0.9967108f,
        0.9980983f,       0.9991120f,       0.9997482f,       1.0000000f,
        0.9998567f,       0.9993046f,       0.9983255f,       0.9968987f,
        0.9950000f,       0.9926005f,       0.9897426f,       0.9864444f,
        0.9827241f,       0.9786000f,       0.9740837f,       0.9691712f,
        0.9638568f,       0.9581349f,       0.9520000f,       0.9454504f,
        0.9384992f,       0.9311628f,       0.9234576f,       0.9154000f,
        0.9070064f,       0.8982772f,       0.8892048f,       0.8797816f,
        0.8700000f,       0.8598613f,       0.8493920f,       0.8386220f,
        0.8275813f,       0.8163000f,       0.8047947f,       0.7930820f,
        0.7811920f,       0.7691547f,       0.7570000f,       0.7447541f,
        0.7324224f,       0.7200036f,       0.7074965f,       0.6949000f,
        0.6822192f,       0.6694716f,       0.6566744f,       0.6438448f,
        0.6310000f,       0.6181555f,       0.6053144f,       0.5924756f,
        0.5796379f,       0.5668000f,       0.5539611f,       0.5411372f,
        0.5283528f,       0.5156323f,       0.5030000f,       0.4904688f,
        0.4780304f,       0.4656776f,       0.4534032f,       0.4412000f,
        0.4290800f,       0.4170360f,       0.4050320f,       0.3930320f,
        0.3810000f,       0.3689184f,       0.3568272f,       0.3447768f,
        0.3328176f,       0.3210000f,       0.3093381f,       0.2978504f,
        0.2865936f,       0.2756245f,       0.2650000f,       0.2547632f,
        0.2448896f,       0.2353344f,       0.2260528f,       0.2170000f,
        0.2081616f,       0.1995488f,       0.1911552f,       0.1829744f,
        0.1750000f,       0.1672235f,       0.1596464f,       0.1522776f,
        0.1451259f,       0.1382000f,       0.1315003f,       0.1250248f,
        0.1187792f,       0.1127691f,       0.1070000f,       0.1014762f,
        0.09618864f,      0.09112296f,      0.08626485f,      0.08160000f,
        0.07712064f,      0.07282552f,      0.06871008f,      0.06476976f,
        0.06100000f,      0.05739621f,      0.05395504f,      0.05067376f,
        0.04754965f,      0.04458000f,      0.04175872f,      0.03908496f,
        0.03656384f,      0.03420048f,      0.03200000f,      0.02996261f,
        0.02807664f,      0.02632936f,      0.02470805f,      0.02320000f,
        0.02180077f,      0.02050112f,      0.01928108f,      0.01812069f,
        0.01700000f,      0.01590379f,      0.01483718f,      0.01381068f,
        0.01283478f,      0.01192000f,      0.01106831f,      0.01027339f,
        0.009533311f,     0.008846157f,     0.008210000f,     0.007623781f,
        0.007085424f,     0.006591476f,     0.006138485f,     0.005723000f,
        0.005343059f,     0.004995796f,     0.004676404f,     0.004380075f,
        0.004102000f,     0.003838453f,     0.003589099f,     0.003354219f,
        0.003134093f,     0.002929000f,     0.002738139f,     0.002559876f,
        0.002393244f,     0.002237275f,     0.002091000f,     0.001953587f,
        0.001824580f,     0.001703580f,     0.001590187f,     0.001484000f,
        0.001384496f,     0.001291268f,     0.001204092f,     0.001122744f,
        0.001047000f,     0.0009765896f,    0.0009111088f,    0.0008501332f,
        0.0007932384f,    0.0007400000f,    0.0006900827f,    0.0006433100f,
        0.0005994960f,    0.0005584547f,    0.0005200000f,    0.0004839136f,
        0.0004500528f,    0.0004183452f,    0.0003887184f,    0.0003611000f,
        0.0003353835f,    0.0003114404f,    0.0002891656f,    0.0002684539f,
        0.0002492000f,    0.0002313019f,    0.0002146856f,    0.0001992884f,
        0.0001850475f,    0.0001719000f,    0.0001597781f,    0.0001486044f,
        0.0001383016f,    0.0001287925f,    0.0001200000f,    0.0001118595f,
        0.0001043224f,    0.00009733560f,   0.00009084587f,   0.00008480000f,
        0.00007914667f,   0.00007385800f,   0.00006891600f,   0.00006430267f,
        0.00006000000f,   0.00005598187f,   0.00005222560f,   0.00004871840f,
        0.00004544747f,   0.00004240000f,   0.00003956104f,   0.00003691512f,
        0.00003444868f,   0.00003214816f,   0.00003000000f,   0.00002799125f,
        0.00002611356f,   0.00002436024f,   0.00002272461f,   0.00002120000f,
        0.00001977855f,   0.00001845285f,   0.00001721687f,   0.00001606459f,
        0.00001499000f,   0.00001398728f,   0.00001305155f,   0.00001217818f,
        0.00001136254f,   0.00001060000f,   0.000009885877f,  0.000009217304f,
        0.000008592362f,  0.000008009133f,  0.000007465700f,  0.000006959567f,
        0.000006487995f,  0.000006048699f,  0.000005639396f,  0.000005257800f,
        0.000004901771f,  0.000004569720f,  0.000004260194f,  0.000003971739f,
        0.000003702900f,  0.000003452163f,  0.000003218302f,  0.000003000300f,
        0.000002797139f,  0.000002607800f,  0.000002431220f,  0.000002266531f,
        0.000002113013f,  0.000001969943f,  0.000001836600f,  0.000001712230f,
        0.000001596228f,  0.000001488090f,  0.000001387314f,  0.000001293400f,
        0.000001205820f,  0.000001124143f,  0.000001048009f,  0.0000009770578f,
        0.0000009109300f, 0.0000008492513f, 0.0000007917212f, 0.0000007380904f,
        0.0000006881098f, 0.0000006415300f, 0.0000005980895f, 0.0000005575746f,
        0.0000005198080f, 0.0000004846123f, 0.0000004518100f
      })
  );
  const std::vector<spectral_sample> CIE_Z(
      sampled_spectrum::create_spectral_samples(CIE_LAMBDAS, {
        0.0006061,    0.0006808792, 0.0007651456, 0.0008600124, 
        0.0009665928, 0.001086,     0.001220586,  0.001372729, 
        0.001543579,  0.001734286,  0.001946,     0.002177777, 
        0.002435809,  0.002731953,  0.003078064,  0.003486, 
        0.003975227, 0.00454088, 0.00515832, 0.005802907, 
        0.006450001, 0.007083216, 0.007745488, 0.008501152, 
        0.009414544, 0.01054999, 0.0119658, 0.01365587, 
        0.01558805, 0.01773015, 0.02005001, 0.02251136, 
        0.02520288, 0.02827972, 0.03189704, 0.03621, 
        0.04143771, 0.04750372, 0.05411988, 0.06099803, 
        0.06785001, 0.07448632, 0.08136156, 0.08915364, 
        0.09854048, 0.1102, 0.1246133, 0.1417017, 
        0.1613035, 0.1832568, 0.2074, 0.2336921, 
        0.2626114, 0.2947746, 0.3307985, 0.3713, 
        0.4162091, 0.4654642, 0.5196948, 0.5795303, 
        0.6456, 0.7184838, 0.7967133, 0.8778459, 
        0.959439, 1.0390501, 1.1153673, 1.1884971, 
        1.2581233, 1.3239296, 1.3856, 1.4426352, 
        1.4948035, 1.5421903, 1.5848807, 1.62296, 
        1.6564048, 1.6852959, 1.7098745, 1.7303821, 
        1.74706, 1.7600446, 1.7696233, 1.7762637, 
        1.7804334, 1.7826, 1.7829682, 1.7816998, 
        1.7791982, 1.7758671, 1.77211, 1.7682589, 
        1.764039, 1.7589438, 1.7524663, 1.7441, 
        1.7335595, 1.7208581, 1.7059369, 1.6887372, 
        1.6692, 1.6475287, 1.6234127, 1.5960223, 
        1.564528, 1.5281, 1.4861114, 1.4395215, 
        1.3898799, 1.3387362, 1.28764, 1.2374223, 
        1.1878243, 1.1387611, 1.090148, 1.0419, 
        0.9941976, 0.9473473, 0.9014531, 0.8566193, 
        0.8129501, 0.7705173, 0.7294448, 0.6899136, 
        0.6521049, 0.6162, 0.5823286, 0.5504162, 
        0.5203376, 0.4919673, 0.46518, 0.4399246, 
        0.4161836, 0.3938822, 0.3729459, 0.3533, 
        0.3348578, 0.3175521, 0.3013375, 0.2861686, 
        0.272, 0.2588171, 0.2464838, 0.2347718, 
        0.2234533, 0.2123, 0.2011692, 0.1901196, 
        0.1792254, 0.1685608, 0.1582, 0.1481383, 
        0.1383758, 0.1289942, 0.1200751, 0.1117, 
        0.1039048, 0.09666748, 0.08998272, 0.08384531, 
        0.07824999, 0.07320899, 0.06867816, 0.06456784, 
        0.06078835, 0.05725001, 0.05390435, 0.05074664, 
        0.04775276, 0.04489859, 0.04216, 0.03950728, 
        0.03693564, 0.03445836, 0.03208872, 0.02984, 
        0.02771181, 0.02569444, 0.02378716, 0.02198925, 
        0.0203, 0.01871805, 0.01724036, 0.01586364, 
        0.01458461, 0.0134, 0.01230723, 0.01130188, 
        0.01037792, 0.009529306, 0.008749999, 0.0080352, 
        0.0073816, 0.0067854, 0.0062428, 0.005749999, 
        0.0053036, 0.0048998, 0.0045342, 0.0042024, 
        0.0039, 0.0036232, 0.0033706, 0.0031414, 
        0.0029348, 0.002749999, 0.0025852, 0.0024386, 
        0.0023094, 0.0021968, 0.0021, 0.002017733, 
        0.0019482, 0.0018898, 0.001840933, 0.0018, 
        0.001766267, 0.0017378, 0.0017112, 0.001683067, 
        0.001650001, 0.001610133, 0.0015644, 0.0015136, 
        0.001458533, 0.0014, 0.001336667, 0.00127, 
        0.001205, 0.001146667, 0.0011, 0.0010688, 
        0.0010494, 0.0010356, 0.0010212, 0.001, 
        0.00096864, 0.00092992, 0.00088688, 0.00084256, 
        0.0008, 0.00076096, 0.00072368, 0.00068592, 
        0.00064544, 0.0006, 0.0005478667, 0.0004916, 
        0.0004354, 0.0003834667, 0.00034, 0.0003072533, 
        0.00028316, 0.00026544, 0.0002518133, 0.00024, 
        0.0002295467, 0.00022064, 0.00021196, 0.0002021867, 
        0.00019, 0.0001742133, 0.00015564, 0.00013596, 
        0.0001168533, 0.0001, 8.613333e-05, 7.46e-05, 
        6.5e-05, 5.693333e-05, 4.999999e-05, 4.416e-05, 
        3.948e-05, 3.572e-05, 3.264e-05, 3e-05, 
        2.765333e-05, 2.556e-05, 2.364e-05, 2.181333e-05, 
        2e-05, 1.813333e-05, 1.62e-05, 1.42e-05, 
        1.213333e-05, 1e-05, 7.733333e-06, 5.4e-06, 
        3.2e-06, 1.333333e-06, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0 
      })
  );

  // Spectral Matching Curves
  // or Spectral Response Curves
  const sampled_spectrum SMC_X(CIE_X);
  const sampled_spectrum SMC_Y(CIE_Y);
  const sampled_spectrum SMC_Z(CIE_Z);
} /* namespace tracer */
