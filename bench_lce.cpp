#include <celero/Celero.h>
#include "lce.hpp"

CELERO_MAIN

class LCEInstance {
   public:
      const size_t m_length;
      const size_t m_position;
      char*const m_stra;
      char*const m_strb;
      size_t index;
      LCEInstance(size_t length)
	 : m_length(length)
	   , m_position(m_length-1)
	   , m_stra(reinterpret_cast<char*>(aligned_alloc(32, m_length+1)))
	   , m_strb(reinterpret_cast<char*>(aligned_alloc(32, m_length+1)))

   {
      random_char rnd_gen;
      std::string input = random_string(rnd_gen, m_length);
      for(size_t i = 0; i < input.size(); ++i) {
	 m_stra[i] = m_strb[i] = input[i];
      }
      m_stra[length] = m_strb[length] = 0;
      m_strb[m_position] = m_stra[m_position]+1;
   }
      // ~LCEInstance() {
	//  free(m_stra);
	//  free(m_strb);
      // }

};

class LCEFixture : public celero::TestFixture {

   const size_t m_instance_length;

   std::vector<celero::TestFixture::ExperimentValue> m_problemspace;
   LCEInstance**const m_instances = nullptr;

   size_t m_current_instance = 0;


   public:

   LCEInstance& instance() const {
      return *m_instances[m_current_instance];
   }

   LCEFixture()
      : m_instance_length(8)
      , m_problemspace(m_instance_length,0)
      , m_instances( new LCEInstance*[m_instance_length])
   {
      for(size_t i = 0; i < m_instance_length; ++i) {
	 m_problemspace[i] = {static_cast<int64_t>(i)};
	 m_instances[i] = new LCEInstance(6 + (2ULL<<(i+4)));
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



// BASELINE_F(LCE, Naive, LCEFixture, 0, 10000)
// {
//    celero::DoNotOptimizeAway(longest_common_prefix_naive(instance().m_stra, instance().m_strb));
// }
BASELINE_F(LCE, Packed, LCEFixture, 0, 10000)
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


