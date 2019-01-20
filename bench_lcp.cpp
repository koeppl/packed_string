#include <celero/Celero.h>
#include "lcp.hpp"
#include "lcpinstance.hpp"
#include "dcheck.hpp"

CELERO_MAIN



class LCPFixture : public celero::TestFixture {

   const size_t m_instance_length;

   std::vector<celero::TestFixture::ExperimentValue> m_problemspace;
   LCPInstance**const m_instances = nullptr;

   size_t m_current_instance = 0;


   public:

   const LCPInstance& instance() const {
      return *m_instances[m_current_instance];
   }

   LCPFixture()
      : m_instance_length(8)
      , m_problemspace(m_instance_length,0)
      , m_instances( new LCPInstance*[m_instance_length])
   {
      for(size_t i = 0; i < m_instance_length; ++i) {
	 m_problemspace[i] = {static_cast<int64_t>(i)};
	 const size_t size = 6 + (2ULL<<(i+4));
	 m_instances[i] = new LCPInstance(size, size-1);
	 DCHECK_LT(static_cast<uint64_t>(m_problemspace[i].Value), m_instance_length);
      }
   }
   ~LCPFixture() {
      for(size_t i = 0; i < m_instance_length; ++i) {
	delete [] m_instances[i];
      }
      delete [] m_instances;
   }


   virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
   {
      return m_problemspace;
   }

   virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override {
      m_current_instance = experimentValue.Value;
      DCHECK_GE(m_instance_length, 0);
      DCHECK_LT(m_current_instance, static_cast<uint64_t>(m_instance_length));
      // const size_t position = experimentValue.Value;
      // m_current_instance = std::min(position, m_instance_length-1); 
      // DCHECK_LT(position, m_instance_length);
   }


};



// BASELINE_F(LCP, Naive, LCPFixture, 0, 10000)
// {
//    celero::DoNotOptimizeAway(longest_common_prefix_naive(instance().m_stra, instance().m_strb));
// }


using namespace packed;

BASELINE_F(LCP, character, LCPFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_character(instance().m_stra, instance().m_strb, instance().m_length));
}

BENCHMARK_F(LCP, packed, LCPFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_packed(instance().m_stra, instance().m_strb, instance().m_length));
}


#ifdef __SSE2__ 
BENCHMARK_F(LCP, sse, LCPFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_sse(instance().m_stra, instance().m_strb, instance().m_length));
}
#endif //SSE2


#ifdef __AVX2__
BENCHMARK_F(LCP, avx2s, LCPFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_avx2_8(instance().m_stra, instance().m_strb, instance().m_length));
}
BENCHMARK_F(LCP, avx2, LCPFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_avx2(instance().m_stra, instance().m_strb, instance().m_length));
}
#endif //avx2


