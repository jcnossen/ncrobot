#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/InputBuffer.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Model.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Vector.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/main.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Matrix.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/CfgParser.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/DebugTrace.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/GNU-MacOSX/ncrobot

dist/Release/GNU-MacOSX/ncrobot: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-MacOSX
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ncrobot ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/InputBuffer.o: nbproject/Makefile-${CND_CONF}.mk ../../InputBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/InputBuffer.o ../../InputBuffer.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Model.o: nbproject/Makefile-${CND_CONF}.mk ../../Model.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Model.o ../../Model.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Vector.o: nbproject/Makefile-${CND_CONF}.mk ../../Vector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Vector.o ../../Vector.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/main.o: nbproject/Makefile-${CND_CONF}.mk ../../main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/main.o ../../main.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Matrix.o: nbproject/Makefile-${CND_CONF}.mk ../../Matrix.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/Matrix.o ../../Matrix.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/CfgParser.o: nbproject/Makefile-${CND_CONF}.mk ../../CfgParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/CfgParser.o ../../CfgParser.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/DebugTrace.o: nbproject/Makefile-${CND_CONF}.mk ../../DebugTrace.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/DebugTrace.o ../../DebugTrace.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/Release
	${RM} dist/Release/GNU-MacOSX/ncrobot

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
