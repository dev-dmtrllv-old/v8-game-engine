FROM ubuntu:20.04

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive && apt-get -y install --no-install-recommends \
	software-properties-common \
	build-essential \
	wget \
	libv8-dev \
	libglfw3-dev \
	nodejs \
	npm \
	xauth 
	
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test && apt install -y g++-11
RUN npm i typescript -g

RUN wget -O glslc.tgz https://storage.googleapis.com/shaderc/artifacts/prod/graphics_shader_compiler/shaderc/linux/continuous_gcc_release/372/20211111-101827/install.tgz
RUN mkdir -p /opt/glslc
RUN tar -xvzf glslc.tgz

ENV PATH="/install/bin:${PATH}"

WORKDIR /nova-engine
CMD ["bash"]
