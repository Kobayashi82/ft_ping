FROM debian:12

RUN apt-get update && apt-get install -y \
    gcc clang build-essential libcap2-bin \
    wget ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp

RUN wget https://ftp.gnu.org/gnu/inetutils/inetutils-2.0.tar.gz \
    && tar -xzf inetutils-2.0.tar.gz \
    && cd inetutils-2.0 \
    && ./configure --prefix=/usr/local \
    && make \
	&& make install \
    && cd / \
    && rm -rf /tmp/inetutils-2.0*

WORKDIR /workspace
RUN useradd -m -s /bin/bash noob
RUN echo 'export PS1="\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ "' >> /root/.bashrc \
    && echo 'alias ls="ls --color=auto"' >> /root/.bashrc \
    && echo 'alias ll="ls -la --color=auto"' >> /root/.bashrc \
    && echo 'alias grep="grep --color=auto"' >> /root/.bashrc
RUN echo 'export PS1="\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ "' >> /home/noob/.bashrc \
    && echo 'alias ls="ls --color=auto"' >> /home/noob/.bashrc \
    && echo 'alias ll="ls -la --color=auto"' >> /home/noob/.bashrc \
    && echo 'alias grep="grep --color=auto"' >> /home/noob/.bashrc

COPY Makefile ./
COPY inc/ ./inc/
COPY src/ ./src/
COPY doc/ ./doc/

COPY <<EOF /entrypoint.sh
#!/bin/bash
if [ -f /usr/local/bin/ping ]; then
    setcap cap_net_raw+ep /usr/local/bin/ping
fi

make # CFLAGS="-Wno-unknown-pragmas"

exec "\$@"
EOF

RUN chmod +x /entrypoint.sh

ENV PATH="/usr/local/bin:$PATH"

ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
