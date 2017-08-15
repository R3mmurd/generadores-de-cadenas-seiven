# include <py-caev-gen.H>

void build_caev_network(const char * const level, const char * const caev,
			unsigned short year, const char * const input,
			const char * const output)
{
  generate_caev_chain(level, caev, year, input, output);
}
