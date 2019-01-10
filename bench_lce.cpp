#include <celero/Celero.h>
#include "lce.hpp"
#include "lceinstance.hpp"

CELERO_MAIN


class LCEFixture : public celero::TestFixture {

   const size_t m_instance_length;

   std::vector<celero::TestFixture::ExperimentValue> m_problemspace;
   LCEInstance**const m_instances = nullptr;

   size_t m_current_instance = 0;


   public:

   const LCEInstance& instance() const {
      return *m_instances[m_current_instance];
   }

   LCEFixture()
      : m_instance_length(8)
      , m_problemspace(m_instance_length,0)
      , m_instances( new LCEInstance*[m_instance_length])
   {
      for(size_t i = 0; i < m_instance_length; ++i) {
	 m_problemspace[i] = {static_cast<int64_t>(i)};
	 const size_t size = 6 + (2ULL<<(i+4));
	 m_instances[i] = new LCEInstance(size, size-1);
	 DCHECK_LT(static_cast<uint64_t>(m_problemspace[i].Value), m_instance_length);
      }
   }
   ~LCEFixture() {
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



BASELINE_F(LCE, Naive, LCEFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_naive(instance().m_stra, instance().m_strb));
}

BENCHMARK_F(LCE, character, LCEFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_character(instance().m_stra, instance().m_length, instance().m_strb, instance().m_length));
}

BENCHMARK_F(LCE, packed, LCEFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_packed(instance().m_stra, instance().m_length, instance().m_strb, instance().m_length));
}


#ifdef __SSE2__ 
BENCHMARK_F(LCE, sse, LCEFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_sse(instance().m_stra, instance().m_length, instance().m_strb, instance().m_length));
}
#endif //SSE2


#ifdef __AVX2__
BENCHMARK_F(LCE, avx2, LCEFixture, 0, 10000)
{
   celero::DoNotOptimizeAway(longest_common_prefix_avx2(instance().m_stra, instance().m_length, instance().m_strb, instance().m_length));
}
#endif //avx2


